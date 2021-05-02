#include <sqee/vk/VulkMesh.hpp>

#include <sqee/core/Utilities.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/misc/Parsing.hpp>

#include <sqee/vk/Helpers.hpp>

using namespace sq;

//============================================================================//

inline void impl_ascii_append_normal(std::byte*& ptr, StringView svx, StringView svy, StringView svz, StringView svw, bool swapYZ)
{
    // swapping yz requires inverting bitangtent sign
    const float fw = svw.empty() ? 0.f : (sv_to_f(svw) * (swapYZ ? -1.f : 1.f));

    // invert tangent to accomodate blender's Y=up tangent space
    // todo: move tangent inversion to the export script
    const float fx = std::round(sv_to_f(svx               ) * 511.f) * (svw.empty() ? 1.f : -1.f);
    const float fy = std::round(sv_to_f(swapYZ ? svz : svy) * 511.f) * (svw.empty() ? 1.f : -1.f);
    const float fz = std::round(sv_to_f(swapYZ ? svy : svz) * 511.f) * (svw.empty() ? 1.f : -1.f);

    const uint32_t uw = (int32_t(fw) &   1) << 30 | (fw < 0.f) << 31;
    const uint32_t ux = (int32_t(fx) & 511) << 20 | (fx < 0.f) << 29;
    const uint32_t uy = (int32_t(fy) & 511) << 10 | (fy < 0.f) << 19;
    const uint32_t uz = (int32_t(fz) & 511) <<  0 | (fz < 0.f) <<  9;

    reinterpret_cast<uint32_t&>(*ptr) = uw | ux | uy | uz;
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
    std::swap(mVertexBuffer, other.mVertexBuffer);
    std::swap(mVertexBufferMem, other.mVertexBufferMem);
    std::swap(mIndexBuffer, other.mIndexBuffer);
    std::swap(mIndexBufferMem, other.mIndexBufferMem);
    mVertexSize = other.mVertexSize;
    mVertexTotal = other.mVertexTotal;
    mIndexTotal = other.mIndexTotal;
    mAttributeFlags = other.mAttributeFlags;
    mSubMeshes = std::move(other.mSubMeshes);
    return *this;
}

VulkMesh::~VulkMesh()
{
    const auto& ctx = VulkanContext::get();
    if (mVertexBuffer) ctx.device.destroy(mVertexBuffer);
    if (mIndexBuffer) ctx.device.destroy(mIndexBuffer);
    if (mVertexBufferMem) mVertexBufferMem.free();
    if (mIndexBufferMem) mIndexBufferMem.free();
}

//============================================================================//

void VulkMesh::load_from_file(const String& path, bool swapYZ)
{
    SQASSERT(!mVertexBuffer, "mesh already loaded");

    if (check_file_exists(path)) impl_load_ascii(path, swapYZ);
    else if (auto _path = path + ".sqm"; check_file_exists(_path)) impl_load_ascii(_path, swapYZ);
    else log_error("Failed to find mesh '{}'", path);
}

//============================================================================//

void VulkMesh::bind_buffers(vk::CommandBuffer cmdbuf) const
{
    cmdbuf.bindVertexBuffers(0u, mVertexBuffer, size_t(0u));
    cmdbuf.bindIndexBuffer(mIndexBuffer, 0u, vk::IndexType::eUint32);
}

void VulkMesh::draw(vk::CommandBuffer cmdbuf, int subMesh) const
{
    if (subMesh >= 0)
    {
        SQASSERT(subMesh < int(mSubMeshes.size()), "invalid submesh");
        const SubMesh& sm = mSubMeshes[subMesh];
        cmdbuf.drawIndexed(sm.indexCount, 1u, sm.firstIndex, 0, 0u);
    }
    else cmdbuf.drawIndexed(mIndexTotal, 1u, 0u, 0, 0u);
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
                    if      (line[i] == "TCRD") mAttributeFlags |= Attribute::TexCoords;
                    else if (line[i] == "NORM") mAttributeFlags |= Attribute::Normals;
                    else if (line[i] == "TANG") mAttributeFlags |= Attribute::Tangents;
                    else if (line[i] == "COLR") mAttributeFlags |= Attribute::Colours;
                    else if (line[i] == "BONE") mAttributeFlags |= Attribute::Bones;

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
                if (mAttributeFlags & Attribute::TexCoords) mVertexSize += sizeof(float[2]);
                if (mAttributeFlags & Attribute::Normals) mVertexSize += sizeof(uint32_t);
                if (mAttributeFlags & Attribute::Tangents) mVertexSize += sizeof(uint32_t);
                if (mAttributeFlags & Attribute::Colours) mVertexSize += sizeof(uint8_t[4]);
                if (mAttributeFlags & Attribute::Bones) mVertexSize += sizeof(int8_t[4]);
                if (mAttributeFlags & Attribute::Bones) mVertexSize += sizeof(uint8_t[4]);

                vertexData.resize(mVertexTotal * mVertexSize);
                vertexPtr = vertexData.data();
                allocatedVertices = true;
            }

            uint index = 0u;

            impl_ascii_append_float(vertexPtr, line[index++]);
            if (swapYZ)
            {
                impl_ascii_append_float(vertexPtr, line[index++ +1u]);
                impl_ascii_append_float(vertexPtr, line[index++ -1u]);
            }
            else
            {
                impl_ascii_append_float(vertexPtr, line[index++]);
                impl_ascii_append_float(vertexPtr, line[index++]);
            }

            if (mAttributeFlags & Attribute::TexCoords)
            {
                impl_ascii_append_float(vertexPtr, line[index++]);
                impl_ascii_append_float(vertexPtr, line[index++]);
            }

            if (mAttributeFlags & Attribute::Normals)
            {
                const auto x = line[index++];
                const auto y = line[index++];
                const auto z = line[index++];
                impl_ascii_append_normal(vertexPtr, x, y, z, {}, swapYZ);
            }

            if (mAttributeFlags & Attribute::Tangents)
            {
                const auto x = line[index++];
                const auto y = line[index++];
                const auto z = line[index++];
                const auto w = line[index++];
                impl_ascii_append_normal(vertexPtr, x, y, z, w, swapYZ);
            }

            if (mAttributeFlags & Attribute::Colours)
            {
                impl_ascii_append_unorm8(vertexPtr, line[index++]);
                impl_ascii_append_unorm8(vertexPtr, line[index++]);
                impl_ascii_append_unorm8(vertexPtr, line[index++]);
                // todo: change colour in file format to rgba
                impl_ascii_append_unorm8(vertexPtr, "1.0");
            }

            if (mAttributeFlags & Attribute::Bones)
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

            // swap indices to make sure winding order is clockwise
            *(indexPtr++) = sv_to_u(line[0]);
            if (swapYZ)
            {
                *(indexPtr++) = sv_to_u(line[2]);
                *(indexPtr++) = sv_to_u(line[1]);
            }
            else
            {
                *(indexPtr++) = sv_to_u(line[1]);
                *(indexPtr++) = sv_to_u(line[2]);
            }
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
    const auto& ctx = VulkanContext::get();

    const auto setup_buffer = [&ctx](vk::Buffer& buffer, VulkanMemory& memory, vk::BufferUsageFlags usage, void* data, size_t size)
    {
        std::tie(buffer, memory) = vk_create_buffer(ctx, size, usage | vk::BufferUsageFlagBits::eTransferDst, false);

        auto staging = StagingBuffer(ctx, size);
        auto cmdbuf = OneTimeCommands(ctx);

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
}

//============================================================================//

VulkMesh::VertexConfig::VertexConfig(vk::Flags<Attribute> flags)
{
    const uint sizePOS = sizeof(float[3]);
    const uint sizeTCRD = sizeof(float[2]) * bool(flags & Attribute::TexCoords);
    const uint sizeNORM = sizeof(uint32_t) * bool(flags & Attribute::Normals);
    const uint sizeTANG = sizeof(uint32_t) * bool(flags & Attribute::Tangents);
    const uint sizeCOLR = sizeof(uint8_t[4]) * bool(flags & Attribute::Colours);
    const uint sizeBONEI = sizeof(int8_t[4]) * bool(flags & Attribute::Bones);
    const uint sizeBONEW = sizeof(uint8_t[4]) * bool(flags & Attribute::Bones);

    const uint offsetTCRD = sizePOS;
    const uint offsetNORM = offsetTCRD + sizeTCRD;
    const uint offsetTANG = offsetNORM + sizeNORM;
    const uint offsetCOLR = offsetTANG + sizeTANG;
    const uint offsetBONEI = offsetCOLR + sizeCOLR;
    const uint offsetBONEW = offsetBONEI + sizeBONEI;

    const uint vertexSize = offsetBONEW + sizeBONEW;

    binding = vk::VertexInputBindingDescription(0u, vertexSize, vk::VertexInputRate::eVertex);

    attributes.push_back({0u, 0u, vk::Format::eR32G32B32Sfloat, 0u});

    if (flags & Attribute::TexCoords)
        attributes.push_back({1u, 0u, vk::Format::eR32G32Sfloat, offsetTCRD});

    if (flags & Attribute::Normals)
        attributes.push_back({2u, 0u, vk::Format::eA2R10G10B10SnormPack32, offsetNORM});

    if (flags & Attribute::Tangents)
        attributes.push_back({3u, 0u, vk::Format::eA2R10G10B10SnormPack32, offsetTANG});

    // todo: use eA2R10G10B10UnormPack32 if alpha is not needed
    if (flags & Attribute::Colours)
        attributes.push_back({4u, 0u, vk::Format::eR8G8B8A8Unorm, offsetCOLR});

    if (flags & Attribute::Bones)
    {
        attributes.push_back({5u, 0u, vk::Format::eR8G8B8A8Sint, offsetBONEI});
        attributes.push_back({6u, 0u, vk::Format::eR8G8B8A8Unorm, offsetBONEW});
    }

    state = vk::PipelineVertexInputStateCreateInfo({}, binding, attributes);
}
