#include <sqee/vk/VulkMesh.hpp>

#include <sqee/core/Utilities.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/misc/Parsing.hpp>

#include <sqee/vk/Helpers.hpp>

using namespace sq;

//============================================================================//

inline void impl_ascii_append_normal(std::byte*& ptr, StringView svx, StringView svy, StringView svz, StringView svw)
{
    const float x = sv_to_f(svx), y = sv_to_f(svy), z = sv_to_f(svz);
    const float w = svw.empty() ? 0.f : sv_to_f(svw);

    const uint32_t xs = x < 0.f, ys = y < 0.f, zs = z < 0.f, ws = w < 0.f;

    const uint32_t value = ws << 31 | (uint32_t(w + (ws << 1)) & 1) << 30 |
                           zs << 29 | (uint32_t(z * 0x1FF + (zs << 9)) & 0x1FF) << 20 |
                           ys << 19 | (uint32_t(y * 0x1FF + (ys << 9)) & 0x1FF) << 10 |
                           xs << 9  | (uint32_t(x * 0x1FF + (xs << 9)) & 0x1FF);

    reinterpret_cast<uint32_t&>(*ptr) = value;
    std::advance(ptr, sizeof(uint32_t));
}

inline void impl_ascii_append_float(std::byte*& ptr, StringView sv)
{
    const auto value = float(sv_to_f(sv));
    reinterpret_cast<float&>(*ptr) = value;
    std::advance(ptr, sizeof(float));
}

inline void impl_ascii_append_unorm8(std::byte*& ptr, StringView sv)
{
    const auto value = uint8_t(sv_to_f(sv) * 255.f);
    reinterpret_cast<uint8_t&>(*ptr) = value;
    std::advance(ptr, sizeof(uint8_t));
}

inline void impl_ascii_append_int8(std::byte*& ptr, StringView sv)
{
    const auto value = int8_t(sv_to_i(sv));
    reinterpret_cast<int8_t&>(*ptr) = value;
    std::advance(ptr, sizeof(int8_t));
}

//============================================================================//

VulkMesh::VulkMesh(VulkMesh&& other)
{
    *this = std::move(other);
}

VulkMesh& VulkMesh::operator=(VulkMesh&& other)
{
    std::swap(ctx, other.ctx);
    std::swap(mVertexBuffer, other.mVertexBuffer);
    std::swap(mVertexBufferMem, other.mVertexBufferMem);
    std::swap(mIndexBuffer, other.mIndexBuffer);
    std::swap(mIndexBufferMem, other.mIndexBufferMem);
    mVertexSize = other.mVertexSize;
    mVertexTotal = other.mVertexTotal;
    mIndexTotal = other.mIndexTotal;
    mAttributeFlags = other.mAttributeFlags;
    mSubMeshes = std::move(other.mSubMeshes);
    mBindingDescription = other.mBindingDescription;
    mAttributeDescriptions = std::move(other.mAttributeDescriptions);
    return *this;
}

VulkMesh::~VulkMesh()
{
    if (ctx == nullptr) return;
    ctx->device.destroy(mVertexBuffer);
    ctx->device.destroy(mIndexBuffer);
    mVertexBufferMem.free();
    mIndexBufferMem.free();
}

//============================================================================//

void VulkMesh::load_from_file(const VulkanContext& _ctx, const String& path, bool swapYZ)
{
    SQASSERT(ctx == nullptr, "mesh already loaded");
    ctx = &_ctx;

    if (check_file_exists(path)) impl_load_ascii(path, swapYZ);
    else if (auto _path = path + ".sqm"; check_file_exists(_path)) impl_load_ascii(_path, swapYZ);
    else log_error("Failed to find mesh '{}'", path);
}

//============================================================================//

vk::PipelineVertexInputStateCreateInfo VulkMesh::get_vertex_input_state_info() const
{
    return vk::PipelineVertexInputStateCreateInfo {
        {}, mBindingDescription, mAttributeDescriptions
    };
}

void VulkMesh::bind_buffers(vk::CommandBuffer cmdbuf) const
{
    cmdbuf.bindVertexBuffers(0u, mVertexBuffer, size_t(0u));
    cmdbuf.bindIndexBuffer(mIndexBuffer, 0u, vk::IndexType::eUint32);
}

void VulkMesh::draw_complete(vk::CommandBuffer cmdbuf) const
{
    cmdbuf.drawIndexed(mIndexTotal, 1u, 0u, 0, 0u);
}

//============================================================================//

void VulkMesh::impl_load_ascii(const String& path, bool swapYZ)
{
    enum class Section { None, Header, Vertices, Indices };
    Section section = Section::None;

    std::vector<std::byte> vertexData;
    std::vector<uint32_t> indexData;

    bool allocatedVertices = false;
    bool allocatedIndices = false;

    std::byte* vertexPtr = nullptr;
    uint32_t* indexPtr = nullptr;

    //--------------------------------------------------------//

    for (const auto& [line, num] : tokenise_file(path).lines)
    {
        const auto& key = line.front();

        if (key.front() == '#') continue;

        if (key == "SECTION")
        {
            if      (line[1] == "Header")   section = Section::Header;
            else if (line[1] == "Vertices") section = Section::Vertices;
            else if (line[1] == "Indices")  section = Section::Indices;

            else log_error("invalid section '{}' in mesh '{}'", line[1], path);
        }

        //--------------------------------------------------------//

        else if (section == Section::Header)
        {
            if (key == "Attrs")
            {
                for (uint i = 1u; i < line.size(); ++i)
                {
                    if      (line[i] == "TCRD") mAttributeFlags |= Attributes::TexCoords;
                    else if (line[i] == "NORM") mAttributeFlags |= Attributes::Normals;
                    else if (line[i] == "TANG") mAttributeFlags |= Attributes::Tangents;
                    else if (line[i] == "COLR") mAttributeFlags |= Attributes::Colours;
                    else if (line[i] == "BONE") mAttributeFlags |= Attributes::Bones;

                    else log_warning("unknown attribute '{}' in mesh '{}'", line[i], path);
                }
            }

            else if (key == "Origin")
            {
                if (swapYZ) sq::parse_tokens(mBounds.origin, line[1], line[3], line[2]);
                else sq::parse_tokens(mBounds.origin, line[1], line[2], line[3]);
            }
            else if (key == "Extents")
            {
                if (swapYZ) sq::parse_tokens(mBounds.extents, line[1], line[3], line[2]);
                else sq::parse_tokens(mBounds.extents, line[1], line[2], line[3]);
            }
            else if (key == "Radius") sq::parse_tokens(mBounds.radius, line[1]);

            else if (key == "SubMesh")
            {
                const uint firstVertex = mVertexTotal;
                const uint firstIndex = mIndexTotal;

                const uint vertexCount = sv_to_u(line[1]);
                const uint indexCount = sv_to_u(line[2]);

                mSubMeshes.push_back({firstVertex, vertexCount, firstIndex, indexCount});

                mVertexTotal += vertexCount;
                mIndexTotal += indexCount;
            }

            else log_warning("unknown header key '{}' in mesh '{}'", key, path);
        }

        //--------------------------------------------------------//

        else if (section == Section::Vertices)
        {
            if (allocatedVertices == false)
            {
                mVertexSize = sizeof(float[3]);
                if (mAttributeFlags & Attributes::TexCoords) mVertexSize += sizeof(float[2]);
                if (mAttributeFlags & Attributes::Normals) mVertexSize += sizeof(uint32_t);
                if (mAttributeFlags & Attributes::Tangents) mVertexSize += sizeof(uint32_t);
                if (mAttributeFlags & Attributes::Colours) mVertexSize += sizeof(uint8_t[4]);
                if (mAttributeFlags & Attributes::Bones) mVertexSize += sizeof(int8_t[4]);
                if (mAttributeFlags & Attributes::Bones) mVertexSize += sizeof(uint8_t[4]);

                vertexData.resize(mVertexTotal * mVertexSize);
                vertexPtr = vertexData.data();
                allocatedVertices = true;
            }

            uint index = 0u;

            impl_ascii_append_float(vertexPtr, line[index++]);
            if (swapYZ)
            {
                impl_ascii_append_float(vertexPtr, line[index+1u]);
                impl_ascii_append_float(vertexPtr, line[index+0u]);
                index += 2u;
            }
            else
            {
                impl_ascii_append_float(vertexPtr, line[index++]);
                impl_ascii_append_float(vertexPtr, line[index++]);
            }

            if (mAttributeFlags & Attributes::TexCoords)
            {
                impl_ascii_append_float(vertexPtr, line[index++]);
                impl_ascii_append_float(vertexPtr, line[index++]);
            }

            if (mAttributeFlags & Attributes::Normals)
            {
                const auto x = line[index++];
                const auto y = line[index++];
                const auto z = line[index++];
                if (swapYZ) impl_ascii_append_normal(vertexPtr, x, z, y, {});
                else impl_ascii_append_normal(vertexPtr, x, y, z, {});
            }

            if (mAttributeFlags & Attributes::Tangents)
            {
                const auto x = line[index++];
                const auto y = line[index++];
                const auto z = line[index++];
                const auto w = line[index++];
                if (swapYZ) impl_ascii_append_normal(vertexPtr, x, z, y, w);
                else impl_ascii_append_normal(vertexPtr, x, y, z, w);
            }

            if (mAttributeFlags & Attributes::Colours)
            {
                impl_ascii_append_unorm8(vertexPtr, line[index++]);
                impl_ascii_append_unorm8(vertexPtr, line[index++]);
                impl_ascii_append_unorm8(vertexPtr, line[index++]);
                // todo: change colour in file format to rgba
                impl_ascii_append_unorm8(vertexPtr, "1.0");
            }

            if (mAttributeFlags & Attributes::Bones)
            {
                impl_ascii_append_int8(vertexPtr, line[index++]);
                impl_ascii_append_int8(vertexPtr, line[index++]);
                impl_ascii_append_int8(vertexPtr, line[index++]);
                impl_ascii_append_int8(vertexPtr, line[index++]);

                impl_ascii_append_unorm8(vertexPtr, line[index++]);
                impl_ascii_append_unorm8(vertexPtr, line[index++]);
                impl_ascii_append_unorm8(vertexPtr, line[index++]);
                impl_ascii_append_unorm8(vertexPtr, line[index++]);
            }
        }

        //--------------------------------------------------------//

        else if (section == Section::Indices)
        {
            if (allocatedIndices == false)
            {
                indexData.resize(mIndexTotal);
                indexPtr = indexData.data();
                allocatedIndices = true;
            }

            *(indexPtr++) = sv_to_u(line[0]);
            *(indexPtr++) = sv_to_u(line[1]);
            *(indexPtr++) = sv_to_u(line[2]);
        }

        //--------------------------------------------------------//

        else log_error("missing SECTION in mesh '{}'", path);
    }

    //--------------------------------------------------------//

    SQASSERT(vertexPtr == std::next(&vertexData.back()), "vertex count mismatch");
    SQASSERT(indexPtr == std::next(&indexData.back()), "index count mismatch");

    impl_load_final(vertexData, indexData);
}

//============================================================================//

void VulkMesh::impl_load_final(std::vector<std::byte>& vertexData, std::vector<uint32_t>& indexData)
{
    const auto setup_buffer = [this](vk::Buffer& buffer, VulkanMemory& memory, vk::BufferUsageFlags usage, void* data, size_t size)
    {
        buffer = ctx->device.createBuffer (
            vk::BufferCreateInfo {
                {}, size, usage | vk::BufferUsageFlagBits::eTransferDst,
                vk::SharingMode::eExclusive, {}
            }
        );

        memory = ctx->allocator->allocate(ctx->device.getBufferMemoryRequirements(buffer), false);
        ctx->device.bindBufferMemory(buffer, memory.get_memory(), memory.get_offset());

        auto staging = StagingBuffer(*ctx, size);
        auto cmdbuf = OneTimeCommands(*ctx);

        std::memcpy(staging.memory.map(), data, size);
        staging.memory.unmap();

        cmdbuf->copyBuffer(staging.buffer, buffer, vk::BufferCopy(0u, 0u, size));
    };

    setup_buffer (
        mVertexBuffer, mVertexBufferMem, vk::BufferUsageFlagBits::eVertexBuffer,
        vertexData.data(), vertexData.size()
    );

    setup_buffer (
        mIndexBuffer, mIndexBufferMem, vk::BufferUsageFlagBits::eIndexBuffer,
        indexData.data(), indexData.size() * 4u
    );

    //--------------------------------------------------------//

    mBindingDescription = vk::VertexInputBindingDescription(0u, mVertexSize, vk::VertexInputRate::eVertex);

    const uint sizePOS = sizeof(float[3]);
    const uint sizeTCRD = sizeof(float[2]) * bool(mAttributeFlags & Attributes::TexCoords);
    const uint sizeNORM = sizeof(uint32_t) * bool(mAttributeFlags & Attributes::Normals);
    const uint sizeTANG = sizeof(uint32_t) * bool(mAttributeFlags & Attributes::Tangents);
    const uint sizeCOLR = sizeof(uint8_t[4]) * bool(mAttributeFlags & Attributes::Colours);
    const uint sizeBONE = sizeof(int8_t[4]) * bool(mAttributeFlags & Attributes::Bones);

    const uint offsetTCRD = sizePOS;
    const uint offsetNORM = offsetTCRD + sizeTCRD;
    const uint offsetTANG = offsetNORM + sizeNORM;
    const uint offsetCOLR = offsetTANG + sizeTANG;

    const uint offsetBoneI = offsetCOLR + sizeCOLR;
    const uint offsetBoneW = offsetBoneI + sizeBONE;

    mAttributeDescriptions.push_back({0u, 0u, vk::Format::eR32G32B32Sfloat, 0u});

    if (mAttributeFlags & Attributes::TexCoords)
        mAttributeDescriptions.push_back({1u, 0u, vk::Format::eR32G32Sfloat, offsetTCRD});

    if (mAttributeFlags & Attributes::Normals)
        mAttributeDescriptions.push_back({2u, 0u, vk::Format::eA2R10G10B10SnormPack32, offsetNORM});

    if (mAttributeFlags & Attributes::Tangents)
        mAttributeDescriptions.push_back({3u, 0u, vk::Format::eA2R10G10B10SnormPack32, offsetTANG});

    if (mAttributeFlags & Attributes::Colours)
        mAttributeDescriptions.push_back({4u, 0u, vk::Format::eR8G8B8A8Unorm, offsetCOLR});

    if (mAttributeFlags & Attributes::Bones)
    {
        mAttributeDescriptions.push_back({5u, 0u, vk::Format::eR8Sint, offsetBoneI});
        mAttributeDescriptions.push_back({6u, 0u, vk::Format::eR8Unorm, offsetBoneW});
    }
}
