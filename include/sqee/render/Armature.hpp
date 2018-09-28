// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/misc/Builtins.hpp>
#include <sqee/maths/Builtins.hpp>

namespace sq {

//============================================================================//

/// The SQEE Armature class.
class SQEE_API Armature final : private MoveOnly
{
public: //====================================================//

    struct Bone
    {
        Vec3F offset   = {};
        float scale    = 1.f;
        QuatF rotation = {};
    };

    using Pose = Vector<Bone>;

    struct Animation
    {
        uint boneCount = 0u;
        uint poseCount = 0u;
        uint totalTime = 0u;

        Vector<uint32_t> times;
        Vector<Pose> poses;
    };

    //--------------------------------------------------------//

    /// Load armature relationship information.
    void load_bones(const String& path, bool swapYZ = false);

    /// Load armature basis information.
    void load_rest_pose(const String& path);

    //--------------------------------------------------------//

    uint get_bone_count() const { return uint(mRestPose.size()); }

    const Pose& get_rest_pose() const { return mRestPose; }

    const Vector<String>& get_bone_names() const { return mBoneNames; }

    //--------------------------------------------------------//

    /// Get the index of a bone by name, -1 on failure
    int32_t get_bone_index(const String& name) const;

    //--------------------------------------------------------//

    Pose make_pose(const String& path) const;

    Animation make_animation(const String& path) const;

    //--------------------------------------------------------//

    Pose blend_poses(const Pose& a, const Pose& b, float factor) const;

    Pose compute_pose(const Animation& animation, float time) const;

     [[deprecated]]
    Vector<Mat34F> compute_ubo_data(const Pose& pose) const;

    void compute_ubo_data(const Pose& pose, Mat34F* out, uint len) const;

    Mat4F compute_transform(const Pose& pose, const String& name) const;

private: //===================================================//

    Vector<String> mBoneNames;
    Vector<int32_t> mBoneParents;

    Vector<Mat4F> mBaseMats;
    Vector<Mat4F> mInverseMats;

    //--------------------------------------------------------//

    Pose mRestPose;

    bool mSwapYZ = false;
};

//============================================================================//

} // namespace sq
