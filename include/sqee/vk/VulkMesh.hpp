// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

#include <sqee/vk/Vulkan.hpp>
#include <sqee/vk/VulkanMemory.hpp>

namespace sq {

//============================================================================//

class SQEE_API VulkMesh final : private MoveOnly
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

    struct SubMesh
    {
        uint firstVertex, vertexCount;
        uint firstIndex, indexCount;
    };

    struct Bounds
    {
        Vec3F origin = {};
        Vec3F extents = {};
        float radius = 0.f;
    };

    //--------------------------------------------------------//

    VulkMesh() = default;

    VulkMesh(VulkMesh&& other);
    VulkMesh& operator=(VulkMesh&& other);

    ~VulkMesh();

    //--------------------------------------------------------//

    /// Load the mesh from an SQM file.
    void load_from_file(const String& path, bool swapYZ = false);

    //--------------------------------------------------------//

    /// Bind vertex and index buffers.
    void bind_buffers(vk::CommandBuffer cmdbuf) const;

    /// Draw the entire mesh.
    void draw_complete(vk::CommandBuffer cmdbuf) const;

    /// Draw the specified submesh.
    void draw_submesh(vk::CommandBuffer, uint index) const;

    //--------------------------------------------------------//

    /// Access vector of sub mesh offsets and sizes.
    const std::vector<SubMesh>& get_sub_meshes() const { return mSubMeshes; }

    /// Check what attributes the mesh contains.
    vk::Flags<Attribute> get_attribute_flags() const { return mAttributeFlags; }

    /// Access the mesh's bounding box.
    const Bounds& get_bounds() const { return mBounds; }

    //--------------------------------------------------------//

    struct VertexConfig
    {
        VertexConfig(vk::Flags<Attribute> flags);

        vk::VertexInputBindingDescription binding;
        std::vector<vk::VertexInputAttributeDescription> attributes;
        vk::PipelineVertexInputStateCreateInfo state;
    };

private: //===================================================//

    vk::Buffer mVertexBuffer;
    sq::VulkanMemory mVertexBufferMem;
    vk::Buffer mIndexBuffer;
    sq::VulkanMemory mIndexBufferMem;

    uint mVertexSize = 0u;
    uint mVertexTotal = 0u;
    uint mIndexTotal = 0u;

    vk::Flags<Attribute> mAttributeFlags;
    std::vector<SubMesh> mSubMeshes;
    Bounds mBounds;

    //--------------------------------------------------------//

    void impl_load_ascii(const String& path, bool swapYZ);

    void impl_load_final(std::vector<std::byte>& vertexData, std::vector<uint32_t>& indexData);
};

//============================================================================//

inline vk::Flags<VulkMesh::Attribute> operator|(VulkMesh::Attribute lhs, VulkMesh::Attribute rhs)
{
    return vk::Flags<VulkMesh::Attribute>(int(lhs) | int(rhs));
}

//============================================================================//

} // namespace sq
