// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

#include <sqee/vk/Vulkan.hpp>
#include <sqee/vk/VulkanContext.hpp>
#include <sqee/vk/VulkanMemory.hpp>

namespace sq {

//============================================================================//

class SQEE_API VulkMesh final : private MoveOnly
{
public: //====================================================//

    enum class Attributes : int8_t
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

//    struct VertexInputStateInfo
//    {
//        vk::VertexInputBindingDescription binding;
//        vk::ArrayProxyNoTemporaries<vk::VertexInputAttributeDescription> attributes;
//    };

    //--------------------------------------------------------//

    VulkMesh() = default;

    VulkMesh(VulkMesh&& other);
    VulkMesh& operator=(VulkMesh&& other);

    ~VulkMesh();

    //--------------------------------------------------------//

    /// Load the mesh from an SQM file.
    void load_from_file(const VulkanContext& ctx, const String& path, bool swapYZ = false);

    //--------------------------------------------------------//

    /// Get information needed to create graphics pipelines.
    vk::PipelineVertexInputStateCreateInfo get_vertex_input_state_info() const;

    //--------------------------------------------------------//

    /// Bind vertex and index buffers.
    void bind_buffers(vk::CommandBuffer cmdbuf) const;

    /// Draw the entire mesh.
    void draw_complete(vk::CommandBuffer cmdbuf) const;

    //--------------------------------------------------------//

    /// Access vector of sub mesh offsets and sizes.
    const std::vector<SubMesh>& get_sub_meshes() const { return mSubMeshes; }

    /// Check what attributes the mesh contains.
    vk::Flags<Attributes> get_attribute_flags() const { return mAttributeFlags; }\

    /// Access the mesh's bounding box.
    const Bounds& get_bounds() const { return mBounds; }

private: //===================================================//

    const VulkanContext* ctx = nullptr;

    vk::Buffer mVertexBuffer;
    sq::VulkanMemory mVertexBufferMem;
    vk::Buffer mIndexBuffer;
    sq::VulkanMemory mIndexBufferMem;

    uint mVertexSize = 0u;
    uint mVertexTotal = 0u;
    uint mIndexTotal = 0u;

    vk::Flags<Attributes> mAttributeFlags;
    std::vector<SubMesh> mSubMeshes;
    Bounds mBounds;

    vk::VertexInputBindingDescription mBindingDescription;
    std::vector<vk::VertexInputAttributeDescription> mAttributeDescriptions;

    //--------------------------------------------------------//

    void impl_load_ascii(const String& path, bool swapYZ);

    void impl_load_final(std::vector<std::byte>& vertexData, std::vector<uint32_t>& indexData);
};

//============================================================================//

} // namespace sq
