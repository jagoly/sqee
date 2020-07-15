// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/misc/Builtins.hpp>
#include <sqee/misc/TinyString.hpp>
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
        QuatF rotation = {};
        Vec3F scale    = {};
    };

    using Pose = Vector<Bone>;

    struct Animation
    {
        uint boneCount = 0u;
        uint poseCount = 0u;
        uint totalTime = 0u;

        Vector<uint32_t> times;
        Vector<Pose> poses;

        bool looping() const { return times.back() != 0u; }
    };

    //--------------------------------------------------------//

    /// Load armature relationship information.
    void load_bones(const String& path, bool swapYZ = false);

    /// Load armature basis information.
    void load_rest_pose(const String& path);

    //--------------------------------------------------------//

    uint get_bone_count() const { return uint(mRestPose.size()); }

    const Pose& get_rest_pose() const { return mRestPose; }

    const Vector<TinyString>& get_bone_names() const { return mBoneNames; }

    //--------------------------------------------------------//

    /// Get the name of a bone by index, empty str on failure
    TinyString get_bone_name(int8_t index) const;

    /// Get the index of a bone by name, -1 on failure
    int8_t get_bone_index(TinyString name) const;

    /// Get the index of a bone's parent, -1 on failure
    int8_t get_bone_parent(int8_t index) const;

    //--------------------------------------------------------//

    Pose make_pose(const String& path) const;

    Animation make_animation(const String& path) const;

    Animation make_null_animation(uint length, bool loop) const;

    //--------------------------------------------------------//

    Pose blend_poses(const Pose& a, const Pose& b, float factor) const;

    Pose compute_pose_continuous(const Animation& animation, float time) const;

    Pose compute_pose_discrete(const Animation& animation, uint time) const;

     [[deprecated]]
    Vector<Mat34F> compute_ubo_data(const Pose& pose) const;

    void compute_ubo_data(const Pose& pose, Mat34F* out, uint len) const;

    //--------------------------------------------------------//

    /// Compute the absolute transform of a bone
    Mat4F compute_bone_matrix(const Pose& pose, int8_t index) const;

    /// Compute the absolute transforms for a pose
    Vector<Mat4F> compute_skeleton_matrices(const Pose& pose) const;

private: //===================================================//

    Vector<TinyString> mBoneNames;
    Vector<int8_t> mBoneParents;

    Vector<Mat4F> mBaseMats;
    Vector<Mat4F> mInverseMats;

    //--------------------------------------------------------//

    Pose mRestPose;

    bool mSwapYZ = false;
};

//============================================================================//

} // namespace sq
