#include <sqee/objects/Armature.hpp>

#include <sqee/core/Algorithms.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/maths/Functions.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/misc/Json.hpp>
#include <sqee/misc/Parsing.hpp>

using namespace sq;

using Animation = Armature::Animation;
using Track = Armature::Animation::Track;

// todo: Single component extra tracks are supported in the format, but
// not properly implemented in SQEE or STS. I may just remove them since
// supporting only vec4 simplifies things for a negligable space cost.

//============================================================================//

static inline std::tuple<float, uint, uint> impl_compute_blend_times(uint frameCount, float time)
{
    const float animTotalTime = float(frameCount);

    time = std::fmod(time, animTotalTime);
    if (std::signbit(time)) time += animTotalTime;

    const float factor = std::modf(time, &time);

    const uint frameA = uint(time);
    const uint frameB = frameA+1u == frameCount ? 0u : frameA+1u;

    return { factor, frameA, frameB };
}

template <class Type>
static inline void impl_compute_blended_value(float factor, uint frameA, uint frameB, const Track& track, Type& ref)
{
    if (track.track.size() > sizeof(Type))
    {
        const Type& valueA = reinterpret_cast<const Type*>(track.track.data())[frameA];
        const Type& valueB = reinterpret_cast<const Type*>(track.track.data())[frameB];

        if constexpr (detail::is_quaternion_v<Type>)
            ref = maths::slerp(valueA, valueB, factor);

        else // scalar or vector
            ref = maths::mix(valueA, valueB, factor);
    }
    else // track is constant
        ref = *reinterpret_cast<const Type*>(track.track.data());
};

template <class Type>
static inline void impl_compute_discrete_value(uint time, const Track& track, Type& ref)
{
    if (track.track.size() > sizeof(Type))
        ref = reinterpret_cast<const Type*>(track.track.data())[time];

    else // track is constant
        ref = *reinterpret_cast<const Type*>(track.track.data());
};

//============================================================================//

void Armature::load_from_file(const String& path)
{
    const JsonValue json = sq::parse_json_from_file(path);

    mRestPose.reserve(json.size());

    mBoneNames.reserve(json.size());
    mBoneParents.reserve(json.size());

    mBaseMats.reserve(json.size());
    mInverseMats.reserve(json.size());

    for (const auto& jb : json)
    {
        mBoneNames.push_back(jb.at("name"));

        if (const auto& parent = jb.at("parent"); parent.is_null() == false)
        {
            const auto iter = algo::find(mBoneNames, TinyString(parent));
            SQASSERT(iter != mBoneNames.end(), "invalid parent");
            mBoneParents.push_back(int8_t(std::distance(mBoneNames.begin(), iter)));
        }
        else mBoneParents.push_back(-1);

        Bone& bone = mRestPose.emplace_back();
        jb.at("offset").get_to(bone.offset);
        jb.at("rotation").get_to(bone.rotation);
        jb.at("scale").get_to(bone.scale);

        bone.rotation = maths::normalize(bone.rotation);

        mBaseMats.push_back(maths::transform(bone.offset, bone.rotation, bone.scale));
        if (mBoneParents.back() >= 0)
            mBaseMats.back() = mBaseMats[size_t(mBoneParents.back())] * mBaseMats.back();

        mInverseMats.push_back(maths::affine_inverse(mBaseMats.back()));
    }
}

//============================================================================//

TinyString Armature::get_bone_name(int8_t index) const
{
    if (index < 0 || size_t(index) >= mBoneNames.size())
        return TinyString();

    return mBoneNames[size_t(index)];
}

int8_t Armature::get_bone_index(TinyString name) const
{
    for (size_t i = 0u; i < mBoneNames.size(); ++i)
        if (mBoneNames[i] == name)
            return int8_t(i);

    return -1;
}

int8_t Armature::get_bone_parent(int8_t index) const
{
    if (index < 0 || size_t(index) > mBoneParents.size())
        return -1;

    return mBoneParents[index];
}

//============================================================================//

Armature::Animation Armature::make_animation(const String& path) const
{
    enum class Section { None, Header, BaseTracks, ExtraTracks };
    Section section = Section::None;

    enum class BaseTrack { None, Offset, Rotation, Scale };
    BaseTrack baseTrack = BaseTrack::None;

    enum class ExtraTrack { None, Float, Vec4F };
    ExtraTrack extraTrack = ExtraTrack::None;

    Animation::Track* track = nullptr;

    Armature::Animation result;

    //--------------------------------------------------------//

    for (const auto& [line, num] : tokenise_file(path).lines)
    {
        const auto& key = line.front();

        if (key.front() == '#') continue;

        if (key == "SECTION")
        {
            if (line[1] == "Header") section = Section::Header;
            else if (line[1] == "BaseTracks") section = Section::BaseTracks;
            else if (line[1] == "ExtraTracks") section = Section::ExtraTracks;
            else log_error("'{}':{}: invalid section", path, num);
        }

        else if (section == Section::Header)
        {
            if (key == "BoneCount")
            {
                result.boneCount = sv_to_u(line[1]);
                if (result.boneCount != get_bone_count())
                    log_error("'{}':{}: bone count mismatch", path, num);
                result.bones.resize(result.boneCount, {});
            }

            else if (key == "FrameCount")
            {
                result.frameCount = sv_to_u(line[1]);
            }

            else log_error("'{}':{}: invalid header key", path, num);
        }

        else if (section == Section::BaseTracks)
        {
            if (key == "TRACK")
            {
                const int8_t boneIndex = get_bone_index(line[1]);
                if (boneIndex == -1)
                    log_error("'{}':{}: invalid bone name", path, num);

                if (line[2] == "offset") baseTrack = BaseTrack::Offset;
                else if (line[2] == "rotation") baseTrack = BaseTrack::Rotation;
                else if (line[2] == "scale") baseTrack = BaseTrack::Scale;
                else log_error("'{}':{}: unknown base track name", path, num);

                track = &result.bones[uint(boneIndex)].emplace_back();
                track->key = line[2];

                if (baseTrack == BaseTrack::Offset)
                {
                    if (line.size() == 6u)
                    {
                        track->track.resize(sizeof(Vec3F) * 1u, {});
                        parse_tokens(*reinterpret_cast<Vec3F*>(track->track.data()), line[3], line[4], line[5]);
                    }
                    else track->track.resize(sizeof(Vec3F) * result.frameCount, {});
                }

                else if (baseTrack == BaseTrack::Rotation)
                {
                    if (line.size() == 7u)
                    {
                        track->track.resize(sizeof(QuatF) * 1u, {});
                        parse_tokens_normalize(*reinterpret_cast<QuatF*>(track->track.data()), line[3], line[4], line[5], line[6]);
                    }
                    else track->track.resize(sizeof(QuatF) * result.frameCount, {});
                }

                else if (baseTrack == BaseTrack::Scale)
                {
                    if (line.size() == 6u)
                    {
                        track->track.resize(sizeof(Vec3F) * 1u, {});
                        parse_tokens(*reinterpret_cast<Vec3F*>(track->track.data()), line[3], line[4], line[5]);

                    }
                    else track->track.resize(sizeof(Vec3F) * result.frameCount, {});
                }
            }

            else
            {
                const uint frame = sv_to_u(key);
                if (frame > result.frameCount)
                    log_error("'{}':{}: frame out of range", path, num);

                if (baseTrack == BaseTrack::Offset)
                    parse_tokens(reinterpret_cast<Vec3F*>(track->track.data())[frame], line[1], line[2], line[3]);

                else if (baseTrack == BaseTrack::Rotation)
                    parse_tokens_normalize(reinterpret_cast<QuatF*>(track->track.data())[frame], line[1], line[2], line[3], line[4]);

                else if (baseTrack == BaseTrack::Scale)
                    parse_tokens(reinterpret_cast<Vec3F*>(track->track.data())[frame], line[1], line[2], line[3]);
            }
        }

        else if (section == Section::ExtraTracks)
        {
            if (key == "TRACK")
            {
                const int8_t boneIndex = get_bone_index(line[1]);
                if (boneIndex == -1)
                    log_error("'{}':{}: invalid bone name", path, num);

                if (line[3] == "Float") extraTrack = ExtraTrack::Float;
                else if (line[3] == "Vec4F") extraTrack = ExtraTrack::Vec4F;
                else log_error("'{}':{}: unknown extra track type", path, num);

                track = &result.bones[uint(boneIndex)].emplace_back();
                track->key = line[2];

                if (extraTrack == ExtraTrack::Float)
                {
                    if (line.size() == 5u)
                    {
                        track->track.resize(sizeof(float) * 1u, {});
                        parse_tokens(*reinterpret_cast<float*>(track->track.data()), line[4]);
                    }
                    else track->track.resize(sizeof(float) * result.frameCount, {});
                }

                else if (extraTrack == ExtraTrack::Vec4F)
                {
                    if (line.size() == 8u)
                    {
                        track->track.resize(sizeof(Vec4F) * 1u, {});
                        parse_tokens(*reinterpret_cast<Vec4F*>(track->track.data()), line[4], line[5], line[6], line[7]);
                    }
                    else track->track.resize(sizeof(Vec4F) * result.frameCount, {});
                }
            }

            else
            {
                const uint frame = sv_to_u(key);
                if (frame > result.frameCount)
                    log_error("'{}':{}: frame out of range", path, num);

                if (extraTrack == ExtraTrack::Float)
                    parse_tokens(reinterpret_cast<float*>(track->track.data())[frame], line[1]);

                else if (extraTrack == ExtraTrack::Vec4F)
                    parse_tokens(reinterpret_cast<Vec4F*>(track->track.data())[frame], line[1], line[2], line[3], line[4]);
            }
        }

        else log_error("'{}': missing SECTION", path);
    }

    //--------------------------------------------------------//

    return result;
}

//============================================================================//

Armature::Animation Armature::make_null_animation(uint length) const
{
    Animation result;

    result.boneCount = get_bone_count();
    result.frameCount = length;

    result.bones.resize(result.boneCount, {});

    for (uint index = 0; index < get_bone_count(); ++index)
    {
        Animation::Track& offsetTrack = result.bones[index].emplace_back();
        offsetTrack.key = "offset";
        offsetTrack.track.resize(sizeof(Vec3F));
        *reinterpret_cast<Vec3F*>(offsetTrack.track.data()) = mRestPose[index].offset;

        Animation::Track& rotationTrack = result.bones[index].emplace_back();
        rotationTrack.key = "rotation";
        rotationTrack.track.resize(sizeof(QuatF));
        *reinterpret_cast<QuatF*>(rotationTrack.track.data()) = mRestPose[index].rotation;

        Animation::Track& scaleTrack = result.bones[index].emplace_back();
        scaleTrack.key = "scale";
        scaleTrack.track.resize(sizeof(Vec3F));
        *reinterpret_cast<Vec3F*>(scaleTrack.track.data()) = mRestPose[index].scale;
    }

    return result;
}

//============================================================================//

Armature::Pose Armature::blend_poses(const Pose& a, const Pose& b, float factor)
{
    SQASSERT(a.size() == b.size(), "bone count mismatch");

    Pose result { a.size() };

    for (uint index = 0u; index < a.size(); ++index)
    {
        result[index].offset = maths::mix(a[index].offset, b[index].offset, factor);
        result[index].rotation = maths::slerp(a[index].rotation, b[index].rotation, factor);
        result[index].scale = maths::mix(a[index].scale, b[index].scale, factor);
    }

    return result;
}

//============================================================================//

Armature::Pose Armature::compute_pose_continuous(const Animation& animation, float time)
{
    const auto [factor, frameA, frameB] = impl_compute_blend_times(animation.frameCount, time);

    Pose result { animation.boneCount };

    for (uint bone = 0u; bone < animation.boneCount; ++bone)
    {
        impl_compute_blended_value(factor, frameA, frameB, animation.bones[bone][0u], result[bone].offset);
        impl_compute_blended_value(factor, frameA, frameB, animation.bones[bone][1u], result[bone].rotation);
        impl_compute_blended_value(factor, frameA, frameB, animation.bones[bone][2u], result[bone].scale);
    }

    return result;
}

Armature::Pose Armature::compute_pose_discrete(const Animation& animation, uint time)
{
    SQASSERT(time < animation.frameCount, "discrete time out of range");

    Pose result { animation.boneCount };

    for (uint bone = 0u; bone < animation.boneCount; ++bone)
    {
        impl_compute_discrete_value(time, animation.bones[bone][0u], result[bone].offset);
        impl_compute_discrete_value(time, animation.bones[bone][1u], result[bone].rotation);
        impl_compute_discrete_value(time, animation.bones[bone][2u], result[bone].scale);
    }

    return result;
}

//============================================================================//

void Armature::compute_extra_continuous(Vec4F& result, const Animation::Track& track, float time)
{
    const uint frameCount = track.track.size() / sizeof(Vec4F);
    const auto [factor, frameA, frameB] = impl_compute_blend_times(frameCount, time);

    impl_compute_blended_value(factor, frameA, frameB, track, result);
}

void Armature::compute_extra_discrete(Vec4F& result, const Animation::Track& track, uint time)
{
    const uint frameCount = track.track.size() / sizeof(Vec4F);
    SQASSERT(frameCount == 1u || time < frameCount, "discrete time out of range");

    impl_compute_discrete_value(time, track, result);
}

//============================================================================//

Mat4F Armature::compute_bone_matrix(const Pose& pose, int8_t index) const
{
    SQASSERT(get_bone_count() == pose.size(), "bone count mismatch");
    SQASSERT(index >= 0 && uint(index) < pose.size(), "invalid bone index");

    Mat4F result = maths::transform(pose[index].offset, pose[index].rotation, pose[index].scale);

    for (int8_t parent = mBoneParents[index]; parent >= 0; parent = mBoneParents[parent])
    {
        const Bone& bone = pose[parent];
        result = maths::transform(bone.offset, bone.rotation, bone.scale) * result;
    }

    return result;
}

//============================================================================//

std::vector<Mat4F> Armature::compute_skeleton_matrices(const Pose& pose) const
{
    SQASSERT(get_bone_count() == pose.size(), "bone count mismatch");

    std::vector<Mat4F> result;
    result.reserve(get_bone_count());

    for (uint i = 0u; i < get_bone_count(); ++i)
    {
        const Bone& bone = pose[i];
        const int8_t parentIndex = mBoneParents[i];

        const Mat4F relMatrix = maths::transform(bone.offset, bone.rotation, bone.scale);

        if (parentIndex < 0) result.push_back(relMatrix);
        else result.push_back(result[parentIndex] * relMatrix);
    }

    return result;
}

//============================================================================//

void Armature::compute_ubo_data(const Pose& pose, Mat34F* out, uint len) const
{
    SQASSERT(get_bone_count() == pose.size(), "bone count mismatch");
    SQASSERT(get_bone_count() <= len, "too many bones for output buffer");

    const std::vector<Mat4F> absMatrices = compute_skeleton_matrices(pose);

    for (uint i = 0u; i < get_bone_count(); ++i)
    {
        const Mat4F skinMatrix = absMatrices[i] * mInverseMats[i];
        out[i] = Mat34F(maths::transpose(skinMatrix));
    }
}

//============================================================================//

const Armature::Animation::Track* Armature::Animation::find_extra(int8_t bone, TinyString key)
{
    SQASSERT(bone >= 0 && uint(bone) < bones.size(), "invalid bone index");

    std::vector<Track>& tracks = bones[uint(bone)];

    // skip over pos/rot/sca tracks
    for (uint i = 3u; i < tracks.size(); ++i)
        if (tracks[i].key == key)
            return &tracks[i];

    return nullptr;
}
