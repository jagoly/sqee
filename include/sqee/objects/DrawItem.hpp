#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/EnumHelper.hpp>
#include <sqee/core/TypeAliases.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/misc/ResourceCache.hpp>
#include <sqee/misc/ResourceHandle.hpp>
#include <sqee/misc/StackString.hpp>

namespace sq {

//====== Forward Declarations ================================================//

using AnimSample = std::vector<std::byte>;

class Armature;
class Mesh;
class Pipeline;
class Texture;

//============================================================================//

/// Used to generate a draw call each frame.
struct SQEE_API DrawItem
{
    using MeshCache = ResourceCache<String, Mesh>;
    using PipelineCache = ResourceCache<String, Pipeline>;
    using TextureCache = ResourceCache<String, Texture>;

    using MeshHandle = Handle<String, Mesh>;
    using PipelineHandle = Handle<String, Pipeline>;
    using TextureHandle = Handle<String, Texture>;

    /// Type of a push constant block member, and how to compute it.
    enum class ParamType : uint8_t
    {
        ConstFloat, ConstFloat2, ConstFloat3, ConstFloat4, // float / vec2 / vec3 / vec4
        ConstInt, ConstInt2, ConstInt3, ConstInt4,         // int / ivec2 / ivec3 / ivec4
        ConstPackedBits,                                   // uint
        ModelMatIndex, NormalMatIndex,                     // uint / uint
        TextureIndex,                                      // uint
        AnimFloat, AnimFloat2, AnimFloat3, AnimFloat4,     // float / vec2 / vec3 / vec4
        AnimInt, AnimInt2, AnimInt3, AnimInt4,             // int / ivec2 / ivec3 / ivec4
        ConstTexTransform, AnimTexTransform,               // mat2x3 / mat2x3
    };

    /// Type of sample value to use for visibility.
    enum class VisDataType : uint8_t
    {
        None,  // always visible
        Float, // 0.0 == not visible
        Int,   // 0 == not visible
    };

    /// Data needed to compute one member of a push constant block.
    struct Param
    {
        ParamType type; uint8_t offset;

        union
        {
            float constFloat; Vec2F constFloat2; Vec3F constFloat3; Vec4F constFloat4;
            int constInt; Vec2I constInt2; Vec3I constInt3; Vec4I constInt4;
            uint32_t constPackedBits; uint matrixIndex; TextureHandle texture;
            uint sampleOffset; float constFloat2x3[2][3]; uint sampleOffsets[3];
        };

        Param()
        {
            std::memset(static_cast<void*>(this), 0, sizeof(Param));
        }

        Param(const Param& other)
        {
            std::memcpy(static_cast<void*>(this), static_cast<const void*>(&other), sizeof(Param));
            if (other.type == ParamType::TextureIndex) new (&texture) TextureHandle(other.texture);
        }

        Param(Param&& other)
        {
            std::memcpy(static_cast<void*>(this), static_cast<const void*>(&other), sizeof(Param));
            if (other.type == ParamType::TextureIndex) new (&texture) TextureHandle(std::move(other.texture));
        }

        Param& operator=(const Param& other)
        {
            Param::~Param();
            std::memcpy(static_cast<void*>(this), static_cast<const void*>(&other), sizeof(Param));
            if (other.type == ParamType::TextureIndex) new (&texture) TextureHandle(other.texture);
            return *this;
        }

        Param& operator=(Param&& other)
        {
            Param::~Param();
            std::memcpy(static_cast<void*>(this), static_cast<const void*>(&other), sizeof(Param));
            if (other.type == ParamType::TextureIndex) new (&texture) TextureHandle(std::move(other.texture));
            return *this;
        }

        ~Param()
        {
            if (type == ParamType::TextureIndex) texture.~TextureHandle();
        }
    };

    TinyString condition;

    MeshHandle mesh;
    PipelineHandle pipeline;

    int16_t order = 0;

    int8_t subMesh = -1;

    VisDataType visDataType = {};
    uint visSampleOffset = 0u;

    std::vector<Param> params;

    /// Load resources and draw items from a Render.json file.
    static std::vector<DrawItem> load_from_json (
        const String& path, const Armature& armature, MeshCache& meshes, PipelineCache& pipelines, TextureCache& textures
    );

    /// Check if the draw item should be drawn.
    bool check_visibility(const AnimSample& sample) const;

    /// Extract params from an animation sample for use as push constants.
    void compute_push_constants (
        const AnimSample& sample, uint modelMatsIndex, uint normalMatsIndex, std::array<std::byte, 128u>& out
    ) const;
};

//============================================================================//

} // namespace sq

SQEE_ENUM_HELPER
(
    sq::DrawItem::ParamType,
    ConstFloat, ConstFloat2, ConstFloat3, ConstFloat4,
    ConstInt, ConstInt2, ConstInt3, ConstInt4,
    ConstPackedBits,
    ModelMatIndex, NormalMatIndex,
    TextureIndex,
    AnimFloat, AnimFloat2, AnimFloat3, AnimFloat4,
    AnimInt, AnimInt2, AnimInt3, AnimInt4,
    ConstTexTransform, AnimTexTransform
)
