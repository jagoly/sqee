// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

#include <sqee/vk/Vulkan.hpp>
#include <sqee/vk/Wrappers.hpp>

namespace sq {

//============================================================================//

/// Asset with vertex layouts, bounding info, and sub meshes.
class SQEE_API Mesh
{
public: //====================================================//

    enum class Attribute
    {
        TexCoords = 0b00001,
        Normals   = 0b00010,
        Tangents  = 0b00100,
        Colours   = 0b01000,
        Bones     = 0b10000
    };

    struct Bounds
    {
        Vec3F origin = {};
        Vec3F extents = {};
        float radius = 0.f;
    };

    struct SubMesh
    {
        TinyString name;
        uint firstVertex, vertexCount;
        uint firstIndex, indexCount;
    };

    using Attributes = vk::Flags<Attribute>;

    //--------------------------------------------------------//

    Mesh() = default;

    SQEE_COPY_DELETE(Mesh)

    Mesh(Mesh&& other);
    Mesh& operator=(Mesh&& other);

    ~Mesh();

    //--------------------------------------------------------//

    /// Load the mesh from an SQM file.
    void load_from_file(const String& path);

    //--------------------------------------------------------//

    /// Get the index of a sub mesh by name, or -1 if not found.
    int8_t get_sub_mesh_index(TinyString name) const noexcept;

    /// Get the index of a sub mesh by name, or -1 if the json is null.
    int8_t json_as_sub_mesh_index(JsonAny json) const;

    //--------------------------------------------------------//

    /// Bind vertex and index buffers.
    void bind_buffers(vk::CommandBuffer cmdbuf) const;

    /// Draw the entire mesh, or a sub mesh.
    void draw(vk::CommandBuffer cmdbuf, int8_t subMesh = -1) const;

    //--------------------------------------------------------//

    /// Check what attributes the mesh contains.
    Attributes get_attributes() const { return mAttributes; }

    /// Access the mesh's bounding information.
    const Bounds& get_bounds() const { return mBounds; }

    //--------------------------------------------------------//

    /// Vertex Layout information, ready for vulkan.
    struct SQEE_API VertexConfig
    {
        VertexConfig(Attributes flags, Attributes ignored);

        vk::VertexInputBindingDescription binding;
        std::vector<vk::VertexInputAttributeDescription> attributes;
        vk::PipelineVertexInputStateCreateInfo state;
    };

private: //===================================================//

    BufferStuff mVertexBuffer;
    BufferStuff mIndexBuffer;

    uint mVertexSize = 0u;
    uint mVertexTotal = 0u;
    uint mIndexTotal = 0u;

    Attributes mAttributes;
    Bounds mBounds;
    std::vector<SubMesh> mSubMeshes;

    //--------------------------------------------------------//

    void impl_load_text(String&& text);

    void impl_load_final(std::vector<std::byte>& vertexData, std::vector<uint32_t>& indexData);
};

//============================================================================//

inline Mesh::Attributes operator|(Mesh::Attribute lhs, Mesh::Attribute rhs)
{
    return Mesh::Attributes(int(lhs) | int(rhs));
}

//============================================================================//

} // namespace sq
