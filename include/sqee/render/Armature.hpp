#pragma once

#include <sqee/builtins.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>
#include <sqee/maths/Matrices.hpp>

namespace sq {

//============================================================================//

/// The SQEE Armature class
class Armature final : public MoveOnly
{
public:

    //========================================================//

    struct PoseBone
    {
        Vec3F offset   = { 0.f, 0.f, 0.f };
        QuatF rotation = { 0.f, 0.f, 0.f, 1.f };
        Vec3F scale    = { 1.f, 1.f, 1.f };
    };

    using Pose = vector<PoseBone>;

    vector<string> mBoneNames;
    vector<int32_t> mBoneParents;

    vector<Mat4F> mBaseMats;
    vector<Mat4F> mInverseMats;

    Pose mRestPose;

    void load_bones_from_file(const string& path);

    void refresh_matrices();

    //========================================================//

    Pose blend_poses(const Pose& poseA, const Pose& poseB, float factor) const;

    Pose load_pose_from_file(const string& path) const;

    vector<Mat34F> pose_to_ubo_data(const Pose& pose) const;
};

//============================================================================//

} // namespace sq
