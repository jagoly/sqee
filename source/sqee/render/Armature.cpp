#include <numeric>

#include <sqee/assert.hpp>
#include <sqee/debug/Logging.hpp>

#include <sqee/maths/Functions.hpp>

#include <sqee/misc/Algorithms.hpp>
#include <sqee/misc/Files.hpp>

#include <sqee/render/Armature.hpp>

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

void Armature::load_bones(const string& path, bool swapYZ)
{
    mSwapYZ = swapYZ;

    //--------------------------------------------------------//

    for (auto& linePair : sq::tokenise_file(path))
    {
        SQASSERT(linePair.first.size() <= 2u, "");

        mBoneNames.push_back(linePair.first[0]);

        if (linePair.first.size() == 2u)
        {
            const auto iter = algo::find(mBoneNames, linePair.first[1]);
            SQASSERT(iter != mBoneNames.end(), "invalid parent bone name");
            mBoneParents.push_back(int(std::distance(mBoneNames.begin(), iter)));
        }
        else mBoneParents.push_back(-1);
    }
}

//============================================================================//

void Armature::load_rest_pose(const string& path)
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

Armature::Pose Armature::make_pose(const string& path) const
{
    const uint boneCount = uint(mBoneNames.size());

    const auto lines = sq::tokenise_file(path);
    SQASSERT(boneCount == lines.size(), "bone count mismatch");

    Armature::Pose result;
    result.reserve(mBoneNames.size());

    for (auto& linePair : lines)
    {
        const auto& line = linePair.first;
        SQASSERT(line.size() == 10u, "invalid pose bone");

        result.emplace_back();

        result.back().offset = { stof(line[0]), stof(line[1]), stof(line[2]) };
        result.back().rotation = { stof(line[3]), stof(line[4]), stof(line[5]), stof(line[6]) };
        result.back().scale = { stof(line[7]), stof(line[8]), stof(line[9]) };
    }

    if (mSwapYZ == true) impl_swap_pose_yz(result);

    return result;
}

//============================================================================//

Armature::Animation Armature::make_animation(const string& path) const
{
    enum class Section { None, Header, Poses };
    Section section = Section::None;

    uint numBones = 0u;

    Armature::Animation result;

    //--------------------------------------------------------//

    for (const auto& [line, num] : tokenise_file(path))
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
            if      (key == "BoneCount") result.boneCount = stou(line[1]);
            else if (key == "TotalTime") result.totalTime = stou(line[1]);

            else if (key == "PoseCount")
            {
                result.poseCount = stou(line[1]);
                result.times.reserve(result.poseCount);
                result.poses.reserve(result.poseCount);
            }

            else if (key == "Times")
            {
                for (uint i = 1u; i < line.size(); ++i)
                    result.times.push_back(stou(line[i]));
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

            result.poses.back().emplace_back();
            Bone& bone = result.poses.back().back();

            bone.offset = { stof(line[0]), stof(line[1]), stof(line[2]) };
            bone.rotation = { stof(line[3]), stof(line[4]), stof(line[5]), stof(line[6]) };
            bone.scale = { stof(line[7]), stof(line[8]), stof(line[9]) };
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
        result.emplace_back();

        result.back().offset = maths::mix(boneA.offset, boneB.offset, factor);
        result.back().rotation = maths::slerp(boneA.rotation, boneB.rotation, factor);
        result.back().scale = maths::mix(boneA.scale, boneB.scale, factor);
    }

    return result;
}

//============================================================================//

Armature::Pose Armature::compute_pose(const Animation& animation, float time) const
{
    SQASSERT(mRestPose.size() == animation.boneCount, "bone count mismatch");

    //--------------------------------------------------------//

    const float animTotalTime = float(animation.totalTime);

    time = std::fmod(time, animTotalTime);
    if (std::signbit(time)) time += animTotalTime;

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

std::vector<Mat34F> Armature::compute_ubo_data(const Pose& pose) const
{
    const uint boneCount = uint(mRestPose.size());

    SQASSERT(boneCount == pose.size(), "bone count mismatch");

    std::vector<Mat4F> localMats;
    std::vector<Mat4F> finalMats;

    for (uint i = 0u; i < boneCount; ++i)
    {
        const Bone& bone = pose[i];
        const int32_t parentIndex = mBoneParents[i];

        localMats.push_back(maths::transform(bone.offset, bone.rotation, bone.scale));

        if (parentIndex >= 0)
        {
            localMats.back() = mBaseMats[uint(parentIndex)] * localMats.back() * mInverseMats[i];
            finalMats.push_back(finalMats[uint(parentIndex)] * localMats.back());
        }
        else
        {
            localMats.back() = localMats.back() * mInverseMats[i];
            finalMats.push_back(localMats.back());
        }
    }

    std::vector<Mat34F> result;
    result.reserve(boneCount);

    for (const Mat4F& mat : finalMats)
        result.push_back(Mat34F(maths::transpose(mat)));

    return result;
}

//============================================================================//

Mat4F Armature::compute_transform(const Pose& pose, const string& name) const
{
    const uint boneCount = uint(mRestPose.size());

    SQASSERT(boneCount == pose.size(), "bone count mismatch");

    const auto iter = algo::find(mBoneNames, name);
    SQASSERT(iter != mBoneNames.end(), "invalid bone name");
    const uint index = uint(std::distance(mBoneNames.begin(), iter));

    const Bone& bone = pose[index];
    const Mat4F local = maths::transform(bone.offset, bone.rotation, bone.scale);

    return mBaseMats[index] * local;
}
