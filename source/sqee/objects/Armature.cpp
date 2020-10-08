#include <sqee/objects/Armature.hpp>

#include <sqee/core/Algorithms.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/maths/Functions.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/misc/Json.hpp>
#include <sqee/misc/Parsing.hpp>

using namespace sq;

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

    enum class BoneTrack { None, Offset, Rotation, Scale };
    BoneTrack boneTrack = BoneTrack::None;

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

                if (line[2] == "offset") boneTrack = BoneTrack::Offset;
                else if (line[2] == "rotation") boneTrack = BoneTrack::Rotation;
                else if (line[2] == "scale") boneTrack = BoneTrack::Scale;
                else log_error("'{}':{}: unknown bone track", path, num);

                track = &result.bones[uint(boneIndex)].emplace_back();
                track->key = line[2];

                if (boneTrack == BoneTrack::Offset)
                {
                    if (line.size() == 6u)
                    {
                        track->track.resize(sizeof(Vec3F) * 1u, {});
                        parse_tokens(*reinterpret_cast<Vec3F*>(track->track.data()), line[3], line[4], line[5]);
                    }
                    else track->track.resize(sizeof(Vec3F) * result.frameCount, {});
                }

                else if (boneTrack == BoneTrack::Rotation)
                {
                    if (line.size() == 7u)
                    {
                        track->track.resize(sizeof(QuatF) * 1u, {});
                        parse_tokens_normalize(*reinterpret_cast<QuatF*>(track->track.data()), line[3], line[4], line[5], line[6]);
                    }
                    else track->track.resize(sizeof(QuatF) * result.frameCount, {});
                }

                else if (boneTrack == BoneTrack::Scale)
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

                if (boneTrack == BoneTrack::Offset)
                    parse_tokens(reinterpret_cast<Vec3F*>(track->track.data())[frame], line[1], line[2], line[3]);

                else if (boneTrack == BoneTrack::Rotation)
                    parse_tokens_normalize(reinterpret_cast<QuatF*>(track->track.data())[frame], line[1], line[2], line[3], line[4]);

                else if (boneTrack == BoneTrack::Scale)
                    parse_tokens(reinterpret_cast<Vec3F*>(track->track.data())[frame], line[1], line[2], line[3]);
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

Armature::Pose Armature::blend_poses(const Pose& a, const Pose& b, float factor) const
{
    SQASSERT(get_bone_count() == a.size(), "bone count mismatch");
    SQASSERT(get_bone_count() == b.size(), "bone count mismatch");

    Pose result { get_bone_count() };

    for (uint index = 0u; index < get_bone_count(); ++index)
    {
        result[index].offset = maths::mix(a[index].offset, b[index].offset, factor);
        result[index].rotation = maths::slerp(a[index].rotation, b[index].rotation, factor);
        result[index].scale = maths::mix(a[index].scale, b[index].scale, factor);
    }

    return result;
}

//============================================================================//

Armature::Pose Armature::compute_pose_continuous(const Animation& animation, float time) const
{
    SQASSERT(get_bone_count() == animation.boneCount, "bone count mismatch");

    const float animTotalTime = float(animation.frameCount);

    time = std::fmod(time, animTotalTime);
    if (std::signbit(time)) time += animTotalTime;

    const float factor = std::modf(time, &time);

    const uint frameA = uint(time);
    const uint frameB = frameA+1u == animation.frameCount ? 0u : frameA+1u;

    const auto get_frame_value = [factor, frameA, frameB](const Animation::Track& track, auto& ref)
    {
        using Type = std::remove_reference_t<decltype(ref)>;

        if (track.track.size() > sizeof(Type))
        {
            const Type& valueA = reinterpret_cast<const Type*>(track.track.data())[frameA];
            const Type& valueB = reinterpret_cast<const Type*>(track.track.data())[frameB];

            if constexpr (detail::is_vector_v<Type>)
                ref = maths::mix(valueA, valueB, factor);

            if constexpr (detail::is_quaternion_v<Type>)
                ref = maths::slerp(valueA, valueB, factor);
        }
        else
            ref = *reinterpret_cast<const Type*>(track.track.data());
    };

    Pose result { animation.boneCount };

    for (uint bone = 0u; bone < animation.boneCount; ++bone)
    {
        get_frame_value(animation.bones[bone][0u], result[bone].offset);
        get_frame_value(animation.bones[bone][1u], result[bone].rotation);
        get_frame_value(animation.bones[bone][2u], result[bone].scale);
    }

    return result;
}

//============================================================================//

Armature::Pose Armature::compute_pose_discrete(const Animation& animation, uint time) const
{
    SQASSERT(get_bone_count() == animation.boneCount, "bone count mismatch");
    SQASSERT(time < animation.frameCount, "discrete time out of range");

    const auto get_frame_value = [time](const Animation::Track& track, auto& ref)
    {
        using Type = std::remove_reference_t<decltype(ref)>;

        if (track.track.size() > sizeof(Type))
            ref = reinterpret_cast<const Type*>(track.track.data())[time];
        else
            ref = *reinterpret_cast<const Type*>(track.track.data());
    };

    Pose result { animation.boneCount };

    for (uint bone = 0u; bone < animation.boneCount; ++bone)
    {
        get_frame_value(animation.bones[bone][0u], result[bone].offset);
        get_frame_value(animation.bones[bone][1u], result[bone].rotation);
        get_frame_value(animation.bones[bone][2u], result[bone].scale);
    }

    return result;
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
