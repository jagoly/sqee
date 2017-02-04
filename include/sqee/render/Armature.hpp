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

    struct Bone
    {
        Vec3F offset   = { 0.f, 0.f, 0.f };
        QuatF rotation = { 0.f, 0.f, 0.f, 1.f };
        Vec3F scale    = { 1.f, 1.f, 1.f };
    };

    struct Frame { uint index, time; };
    using Pose = vector<Bone>;

    struct Animation
    {
        uint boneCount = 0u;
        uint tickRate = 0u;
        vector<Frame> frames;
        vector<Pose> poses;
    };

    //========================================================//

    void load_bones(const string& path);

    void load_rest_pose(const string& path);

    //========================================================//

    Pose make_pose(const string& path) const;

    Animation make_animation(const string& path) const;

    //========================================================//

    Pose blend_poses(const Pose& a, const Pose& b, float factor) const;

    Pose compute_pose(const Animation& animation, float time) const;

    vector<Mat34F> compute_ubo_data(const Pose& pose) const;

    Mat4F compute_transform(const Pose& pose, const string& name) const;

private:

    //========================================================//

    vector<string> mBoneNames;
    vector<int32_t> mBoneParents;

    vector<Mat4F> mBaseMats;
    vector<Mat4F> mInverseMats;

    Pose mRestPose;
};

//============================================================================//

} // namespace sq
