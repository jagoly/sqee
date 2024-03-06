#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/EnumHelper.hpp>
#include <sqee/core/TypeAliases.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Quaternion.hpp>
#include <sqee/misc/StackString.hpp>
#include <sqee/vk/Vulkan.hpp> // Flags

namespace sq { //###############################################################

/// A set of tracks that can be used to get an AnimSample.
struct SQEE_API Animation
{
    struct SampleTime
    {
        uint frameA;  ///< Index of the first frame.
        uint frameB;  ///< Index of the second frame.
        float factor; ///< Mix factor between the frames.
    };

    Animation() = default;

    SQEE_COPY_DELETE(Animation)
    SQEE_MOVE_DEFAULT(Animation)

    uint frameCount = 0u;
    //uint flags = 0u;

    std::vector<std::byte> bytes;
    std::vector<std::span<const std::byte>> tracks;

    SampleTime compute_sample_time(float time) const;
};

/// A sample from every bone and block track in an armature.
using AnimSample = std::vector<std::byte>;

//==============================================================================

/// Asset defining bones and other animation tracks.
class SQEE_API Armature
{
public: //======================================================

    enum class TrackType : uint8_t
    {
        Float, Float2, Float3, Float4, // linear interpolation
        Int, Int2, Int3, Int4,         // no interpolation
        Angle, Quaternion              // fancy interpolation
    };

    enum class BoneFlag : uint8_t
    {
        Billboard = 1 << 0, ///< Bone should always face the camera.
    };

    using BoneFlags = vk::Flags<BoneFlag>;

    struct BoneInfo
    {
        int8_t parent;
        BoneFlags flags;
    };

    struct TrackInfo
    {
        int8_t boneIndex;
        int8_t blockIndex;
        TrackType type;
        uint32_t offset;
    };

    struct Bone
    {
        Vec3F offset;
        QuatF rotation;
        Vec3F scale;
    };

    //----------------------------------------------------------

    Armature() = default;
    Armature(const String& path) { load_from_file(path); }

    SQEE_COPY_DELETE(Armature)
    SQEE_MOVE_DEFAULT(Armature)

    /// Load the armature from JSON.
    void load_from_file(const String& path);

    //----------------------------------------------------------

    size_t get_bone_count() const { return mBoneInfos.size(); }

    const AnimSample& get_rest_sample() const { return mRestSample; }

    const std::vector<BoneInfo>& get_bone_infos() const { return mBoneInfos; }

    const std::vector<TrackInfo>& get_track_infos() const { return mTrackInfos; }

    const std::vector<TinyString>& get_bone_names() const { return mBoneNames; }

    const std::vector<TinyString>& get_block_names() const { return mBlockNames; }

    const std::vector<TinyString>& get_track_names() const { return mBlockNames; }

    //----------------------------------------------------------

    /// Get the index of a bone by name, or -1 if not found.
    int8_t get_bone_index(TinyString name) const noexcept;

    /// Get the index of a block by name, or -1 if not found.
    int8_t get_block_index(TinyString name) const noexcept;

    /// Get the index of a block track by name, or -1 if not found.
    int16_t get_block_track_index(TinyString blockName, TinyString trackName) const noexcept;

    /// Get the index of a bone by name, or -1 if the json is null.
    int8_t json_as_bone_index(JsonAny json) const;

    /// Get the name of a bone by index, or null if the index is -1.
    JsonMutAny json_from_bone_index(JsonMutDocument& document, int8_t index) const noexcept;

    //----------------------------------------------------------

    /// Load an associated animation from an SQA file.
    Animation load_animation_from_file(const String& path) const;

    /// Generate an animation contaning N frames of the rest pose.
    Animation make_null_animation(uint frameCount) const;

    //----------------------------------------------------------

    /// Sample an animation at the given time.
    void compute_sample(const Animation& animation, float time, AnimSample& out) const;

    /// Blend between two animation samples.
    void blend_samples(const AnimSample& a, const AnimSample& b, float factor, AnimSample& out) const;

    //----------------------------------------------------------

    // Bone matrices are absolute transforms. Used to transform objects not
    // bound to this armature, or for drawing a skeleton.
    //
    // Model matrices are relative to each bone's rest transform. Mainly used to
    // transform the vertices of a mesh (vertex skinning).
    //
    // todo: Are there better terms for these? Using the same terms as blender
    //       would probably be a good idea.

    /// Compute the absolute transform of a bone.
    Mat4F compute_bone_matrix(const AnimSample& sample, uint8_t index) const;

    /// Compute the absolute transforms of all bones.
    std::vector<Mat4F> compute_bone_matrices(const AnimSample& sample) const;

    /// Compute the transform of a bone.
    Mat4F compute_model_matrix(const AnimSample& sample, Mat4F modelMatrix, uint8_t index) const;

    /// Compute the transforms of all bones, transposed for shaders.
    void compute_model_matrices (
        const AnimSample& sample, Mat4F viewMatrix, Mat4F invViewMatrix, Mat4F modelMatrix, Vec2F billboardScale, Mat34F* modelMats, size_t len
    ) const;

private: //=====================================================

    AnimSample mRestSample;

    std::vector<BoneInfo> mBoneInfos;

    std::vector<Mat4F> mBaseMats;
    std::vector<Mat4F> mInverseMats;

    std::vector<TrackInfo> mTrackInfos;

    std::vector<TinyString> mBoneNames;
    std::vector<TinyString> mBlockNames;
    std::vector<TinyString> mTrackNames;

    //----------------------------------------------------------

    Animation impl_load_animation_text(String&& text) const;
};

} // namespace sq ##############################################################

SQEE_ENUM_HELPER
(
    sq::Armature::TrackType,
    Float, Float2, Float3, Float4,
    Int, Int2, Int3, Int4,
    Angle, Quaternion
)
