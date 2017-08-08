#pragma once

#include <sqee/builtins.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>
#include <sqee/maths/Matrices.hpp>

namespace sq {

//============================================================================//

/// The SQEE Armature class.
class Armature final : public MoveOnly
{
public: //====================================================//

    struct Bone
    {
        Vec3F offset   = { 0.f, 0.f, 0.f };
        QuatF rotation = { 0.f, 0.f, 0.f, 1.f };
        Vec3F scale    = { 1.f, 1.f, 1.f };
    };

    using Pose = std::vector<Bone>;

    struct Animation
    {
        uint boneCount = 0u;
        uint poseCount = 0u;
        uint totalTime = 0u;

        std::vector<uint32_t> times;
        std::vector<Pose> poses;
    };

    //--------------------------------------------------------//

    /// Load armature relationship information.
    void load_bones(const string& path, bool swapYZ = false);

    /// Load armature basis information.
    void load_rest_pose(const string& path);

    //--------------------------------------------------------//

    const Pose& get_rest_pose() const { return mRestPose; }

    //--------------------------------------------------------//

    Pose make_pose(const string& path) const;

    Animation make_animation(const string& path) const;

    //--------------------------------------------------------//

    Pose blend_poses(const Pose& a, const Pose& b, float factor) const;

    Pose compute_pose(const Animation& animation, float time) const;

    std::vector<Mat34F> compute_ubo_data(const Pose& pose) const;

    Mat4F compute_transform(const Pose& pose, const string& name) const;

private: //===================================================//

    std::vector<string> mBoneNames;
    std::vector<int32_t> mBoneParents;

    std::vector<Mat4F> mBaseMats;
    std::vector<Mat4F> mInverseMats;

    //--------------------------------------------------------//

    Pose mRestPose;

    bool mSwapYZ = false;
};

//============================================================================//

} // namespace sq
