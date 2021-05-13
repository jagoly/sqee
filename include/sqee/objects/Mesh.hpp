// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

#include <sqee/vk/Vulkan.hpp>
#include <sqee/vk/VulkanMemory.hpp>

namespace sq {

//============================================================================//

class SQEE_API Mesh final : private MoveOnly
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
        uint firstVertex, vertexCount;
        uint firstIndex, indexCount;
    };

    using Attributes = vk::Flags<Attribute>;

    //--------------------------------------------------------//

    Mesh() = default;

    Mesh(Mesh&& other);
    Mesh& operator=(Mesh&& other);

    ~Mesh();

    //--------------------------------------------------------//

    /// Load the mesh from an SQM file.
    void load_from_file(const String& path, bool swapYZ = false);

    //--------------------------------------------------------//

    /// Bind vertex and index buffers.
    void bind_buffers(vk::CommandBuffer cmdbuf) const;

    /// Draw the entire mesh or a submesh.
    void draw(vk::CommandBuffer cmdbuf, int subMesh = -1) const;

    //--------------------------------------------------------//

    /// Access vector of sub mesh offsets and sizes.
    const std::vector<SubMesh>& get_sub_meshes() const { return mSubMeshes; }

    /// Check what attributes the mesh contains.
    Attributes get_attributes() const { return mAttributes; }

    /// Access the mesh's bounding box.
    const Bounds& get_bounds() const { return mBounds; }

    //--------------------------------------------------------//

    struct VertexConfig
    {
        VertexConfig(Attributes flags);

        vk::VertexInputBindingDescription binding;
        std::vector<vk::VertexInputAttributeDescription> attributes;
        vk::PipelineVertexInputStateCreateInfo state;
    };

private: //===================================================//

    vk::Buffer mVertexBuffer;
    VulkanMemory mVertexBufferMem;
    vk::Buffer mIndexBuffer;
    VulkanMemory mIndexBufferMem;

    uint mVertexSize = 0u;
    uint mVertexTotal = 0u;
    uint mIndexTotal = 0u;

    Attributes mAttributes;
    Bounds mBounds;
    std::vector<SubMesh> mSubMeshes;

    //--------------------------------------------------------//

    void impl_load_text(String&& text, bool swapYZ);

    void impl_load_final(std::vector<std::byte>& vertexData, std::vector<uint32_t>& indexData);
};

//============================================================================//

inline Mesh::Attributes operator|(Mesh::Attribute lhs, Mesh::Attribute rhs)
{
    return Mesh::Attributes(int(lhs) | int(rhs));
}

//============================================================================//

} // namespace sq
