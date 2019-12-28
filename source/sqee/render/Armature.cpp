// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#include <sqee/render/Armature.hpp>

#include <numeric>

#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/maths/Functions.hpp>
#include <sqee/misc/Algorithms.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/misc/Parsing.hpp>

using namespace sq;

//============================================================================//

namespace { // anonymous

inline void impl_swap_pose_yz(Armature::Pose& pose)
{
    for (Armature::Bone& bone : pose)
    {
        std::swap(bone.offset.y, bone.offset.z);
        std::swap(bone.rotation.y, bone.rotation.z);

        bone.rotation = maths::conjugate(bone.rotation);
    }
}

} // anonymous namespace

//============================================================================//

void Armature::load_bones(const String& path, bool swapYZ)
{
    mSwapYZ = swapYZ;

    //--------------------------------------------------------//

    for (auto& [line, num] : sq::tokenise_file(path).lines)
    {
        SQASSERT(line.size() <= 2u, "");

        mBoneNames.push_back(String(line[0]));

        if (line.size() == 2u)
        {
            const auto iter = algo::find(mBoneNames, line[1]);
            SQASSERT(iter != mBoneNames.end(), "invalid parent bone name");
            mBoneParents.push_back(int(std::distance(mBoneNames.begin(), iter)));
        }
        else mBoneParents.push_back(-1);
    }
}

//============================================================================//

void Armature::load_rest_pose(const String& path)
{
    mRestPose = make_pose(path);

    const uint boneCount = uint(mRestPose.size());

    mBaseMats.reserve(boneCount);
    mInverseMats.reserve(boneCount);

    for (uint i = 0u; i < boneCount; ++i)
    {
        const Bone& bone = mRestPose[i];
        const int32_t parentIndex = mBoneParents[i];

        mBaseMats.push_back(maths::transform(bone.offset, bone.rotation, bone.scale));

        if (parentIndex >= 0)
            mBaseMats.back() = mBaseMats[uint(parentIndex)] * mBaseMats.back();

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

//============================================================================//

Armature::Pose Armature::make_pose(const String& path) const
{
    const uint boneCount = uint(mBoneNames.size());

    const auto tokenFile = sq::tokenise_file(path);
    SQASSERT(boneCount == tokenFile.lines.size(), "bone count mismatch");

    Armature::Pose result;
    result.reserve(boneCount);

    for (const auto& [line, num] : tokenFile.lines)
    {
        Bone& bone = result.emplace_back();

        // todo: remove support for old pose format
        if (line.size() == 10)
        {
            sq::parse_tokens(bone.offset, line[0], line[1], line[2]);
            sq::parse_tokens(bone.rotation, line[3], line[4], line[5], line[6]);
            sq::parse_tokens(bone.scale, line[7]);
        }
        else if (line.size() == 8)
        {
            sq::parse_tokens(bone.offset, line[0], line[1], line[2]);
            sq::parse_tokens(bone.scale, line[3]);
            sq::parse_tokens(bone.rotation, line[4], line[5], line[6], line[7]);
        }
        else log_error("invalid bone on line %d of pose '%s'", num, path);
    }

    if (mSwapYZ == true) impl_swap_pose_yz(result);

    return result;
}

//============================================================================//

Armature::Animation Armature::make_animation(const String& path) const
{
    enum class Section { None, Header, Poses };
    Section section = Section::None;

    uint numBones = 0u;

    Armature::Animation result;

    //--------------------------------------------------------//

    for (const auto& [line, num] : tokenise_file(path).lines)
    {
        const auto& key = line.front();

        //--------------------------------------------------------//

        if (key.front() == '#') continue;

        //--------------------------------------------------------//

        if (key == "SECTION")
        {
            if      (line[1] == "Header") section = Section::Header;
            else if (line[1] == "Poses")  section = Section::Poses;

            else log_error("invalid section '%s' in animation '%s'", line[1], path);
        }

        //--------------------------------------------------------//

        else if (section == Section::Header)
        {
            if      (key == "BoneCount") result.boneCount = sv_to_u(line[1]);
            else if (key == "TotalTime") result.totalTime = sv_to_u(line[1]);

            else if (key == "PoseCount")
            {
                result.poseCount = sv_to_u(line[1]);
                result.times.reserve(result.poseCount);
                result.poses.reserve(result.poseCount);
            }

            else if (key == "Times")
            {
                for (uint i = 1u; i < line.size(); ++i)
                {
                    result.times.push_back(sv_to_u(line[i]));

                    if (result.times.back() == 0u && i != line.size() - 1u)
                        log_error("zero time for not last pose in animation '%s'", path);
                }
            }

            else log_warning("unknown header key '%s' in animation '%s'", key, path);
        }

        //--------------------------------------------------------//

        else if (section == Section::Poses)
        {
            if (numBones++ % result.boneCount == 0u)
            {
                result.poses.emplace_back();
                result.poses.back().reserve(result.boneCount);
            }

            //log_assert(line.size() == 10u, "invalid bone on line %d of animation '%s'", num, path);

            Bone& bone = result.poses.back().emplace_back();

            if (line.size() == 10)
            {
                sq::parse_tokens(bone.offset, line[0], line[1], line[2]);
                sq::parse_tokens(bone.rotation, line[3], line[4], line[5], line[6]);
                sq::parse_tokens(bone.scale, line[7]);
            }
            else if (line.size() == 8)
            {
                sq::parse_tokens(bone.offset, line[0], line[1], line[2]);
                sq::parse_tokens(bone.scale, line[3]);
                sq::parse_tokens(bone.rotation, line[4], line[5], line[6], line[7]);
            }
            else log_error("invalid bone on line %d of animation '%s'", num, path);
        }

        //--------------------------------------------------------//

        else log_error("missing SECTION in animation '%s'", path);
    }

    //--------------------------------------------------------//

    #ifdef SQEE_DEBUG

    log_assert(mRestPose.size() == result.boneCount, "bone count mismatch in '%s'", path);
    log_assert(numBones == result.poseCount * result.boneCount, "missing or extra bones in '%s'", path);

    log_assert(result.times.size() == result.poseCount, "time count mismatch in '%s'", path);
    log_assert(result.poses.size() == result.poseCount, "pose count mismatch in '%s'", path);

    const uint totalTime = std::accumulate(result.times.begin(), result.times.end(), 0u);
    log_assert(totalTime == result.totalTime, "times do not add up to total in '%s'", path);

    #endif

    //--------------------------------------------------------//

    if (mSwapYZ == true)
    {
        for (Pose& pose : result.poses)
            impl_swap_pose_yz(pose);
    }

    //--------------------------------------------------------//

    return result;
}

//============================================================================//

Armature::Pose Armature::blend_poses(const Pose& a, const Pose& b, float factor) const
{
    const uint boneCount = uint(mRestPose.size());

    SQASSERT(boneCount == a.size(), "bone count mismatch");
    SQASSERT(boneCount == b.size(), "bone count mismatch");

    Armature::Pose result;
    result.reserve(boneCount);

    for (uint i = 0u; i < boneCount; ++i)
    {
        const Bone& boneA = a[i];
        const Bone& boneB = b[i];

        Bone& bone = result.emplace_back();

        bone.offset = maths::mix(boneA.offset, boneB.offset, factor);
        bone.scale = maths::mix(boneA.scale, boneB.scale, factor);
        bone.rotation = maths::slerp(boneA.rotation, boneB.rotation, factor);
    }

    return result;
}

//============================================================================//

Armature::Pose Armature::compute_pose(const Animation& animation, float time) const
{
    SQASSERT(mRestPose.size() == animation.boneCount, "bone count mismatch");

    //--------------------------------------------------------//

    if (animation.poseCount == 1)
    {
        return animation.poses.front();
    }

    //--------------------------------------------------------//

    const float animTotalTime = float(animation.totalTime);

    if (animation.times.back() == 0u)
    {
        time = maths::clamp(time, 0.f, animTotalTime);
    }
    else
    {
        time = std::fmod(time, animTotalTime);
        if (std::signbit(time)) time += animTotalTime;
    }

    //--------------------------------------------------------//

    uint indexA = 0u, indexB = 1u;
    uint timeA = 0u, timeB = animation.times[0];

    while (float(timeB) <= time)
    {
        indexA = indexB;
        timeA = timeB;

        if (++indexB == animation.poseCount) indexB = 0u;
        timeB += animation.times[indexA];
    }

    //--------------------------------------------------------//

    const Pose& poseA = animation.poses[indexA];
    const Pose& poseB = animation.poses[indexB];

    const float factor = float(time - timeA) / float(timeB - timeA);

    return blend_poses(poseA, poseB, factor);
}

//============================================================================//

Vector<Mat34F> Armature::compute_ubo_data(const Pose& pose) const
{
    Vector<Mat34F> result;
    result.resize(pose.size());

    compute_ubo_data(pose, result.data(), pose.size());

    return result;
}

void Armature::compute_ubo_data(const Pose& pose, Mat34F* out, uint len) const
{
    const uint boneCount = uint(mRestPose.size());

    SQASSERT(boneCount == pose.size(), "bone count mismatch");
    SQASSERT(boneCount <= len, "too many bones for output buffer");

    Vector<Mat4F> absMatrices;
    absMatrices.reserve(boneCount);

    for (uint i = 0u; i < boneCount; ++i)
    {
        const Bone& bone = pose[i];
        const int32_t parentIndex = mBoneParents[i];

        const Mat4F relMatrix = maths::transform(bone.offset, bone.rotation, bone.scale);

        if (parentIndex < 0) absMatrices.push_back(relMatrix);
        else absMatrices.push_back(absMatrices[parentIndex] * relMatrix);
    }

    for (uint i = 0u; i < boneCount; ++i)
    {
        const Mat4F skinMatrix = absMatrices[i] * mInverseMats[i];
        out[i] = Mat34F(maths::transpose(skinMatrix));
    }
}

//============================================================================//

Mat4F Armature::compute_transform(const Pose& pose, uint index) const
{
    SQASSERT(get_bone_count() == pose.size(), "bone count mismatch");
    SQASSERT(index < pose.size(), "invalid bone index");

    const Bone& bone = pose[index];
    const Mat4F localMat = maths::transform(bone.offset, bone.rotation, bone.scale);

    return mBaseMats[index] * localMat;
}

Mat4F Armature::compute_transform(const Pose& pose, TinyString name) const
{
    const int8_t boneIndex = get_bone_index(name);
    SQASSERT(boneIndex >= 0, "invalid bone name");

    return compute_transform(pose, uint(boneIndex));
}
