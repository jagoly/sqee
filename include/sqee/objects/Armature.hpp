// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

namespace sq {

//============================================================================//

/// Asset defining a skeleton for animation.
///
/// This class contains no vulkan objects, a seperate UBO is required for use.
///
class SQEE_API Armature final : private MoveOnly
{
public: //====================================================//

    /// Local transform for a single bone.
    struct Bone
    {
        Vec3F offset { nullptr };
        QuatF rotation { nullptr };
        Vec3F scale { nullptr };
    };

    /// Local transforms for all bones in an armature.
    using Pose = std::vector<Bone>;

    //--------------------------------------------------------//

    /// A set of tracks that can be sampled to get a Pose.
    struct SQEE_API Animation
    {
        struct Track
        {
            TinyString key;
            std::vector<std::byte> track;
        };

        uint boneCount = 0u;
        uint frameCount = 0u;
        std::vector<std::vector<Track>> bones;

        /// Helper to access an extra track by name.
        const Track* find_extra(int8_t bone, TinyString key) const;
    };

    //--------------------------------------------------------//

    /// Load bones and rest pose from a json file.
    void load_from_file(const String& path);

    //--------------------------------------------------------//

    size_t get_bone_count() const { return mRestPose.size(); }

    const Pose& get_rest_pose() const { return mRestPose; }

    const std::vector<TinyString>& get_bone_names() const { return mBoneNames; }

    //--------------------------------------------------------//

    /// Get the name of a bone by index, empty str on failure.
    TinyString get_bone_name(int8_t index) const;

    /// Get the index of a bone by name, -1 on failure.
    int8_t get_bone_index(TinyString name) const;

    /// Get the index of a bone's parent, -1 on failure.
    int8_t get_bone_parent(int8_t index) const;

    //--------------------------------------------------------//

    Animation load_animation_from_file(const String& path) const;

    Animation make_null_animation(uint length) const;

    //--------------------------------------------------------//

    // todo: poses are big objects and should probably not be reallocated constantly

    static Pose blend_poses(const Pose& a, const Pose& b, float factor);

    static Pose compute_pose_continuous(const Animation& animation, float time);

    static void compute_extra_continuous(Vec4F& result, const Animation::Track& track, float time);

    static Pose compute_pose_discrete(const Animation& animation, uint time);

    static void compute_extra_discrete(Vec4F& result, const Animation::Track& track, uint time);

    //--------------------------------------------------------//

    /// Compute the absolute transform of a bone.
    Mat4F compute_bone_matrix(const Pose& pose, int8_t index) const;

    /// Compute the absolute transforms for a pose.
    std::vector<Mat4F> compute_skeleton_matrices(const Pose& pose) const;

    /// Compute transposed pose matrices, ready for a shader.
    void compute_ubo_data(const Pose& pose, Mat34F* out, size_t len) const;

private: //===================================================//

    Pose mRestPose;

    std::vector<TinyString> mBoneNames;
    std::vector<int8_t> mBoneParents;

    std::vector<Mat4F> mBaseMats;
    std::vector<Mat4F> mInverseMats;

    //--------------------------------------------------------//

    Animation impl_load_animation_text(String&& text) const;
};

//============================================================================//

} // namespace sq
