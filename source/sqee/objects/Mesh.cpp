#include <sqee/objects/Mesh.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/misc/Parsing.hpp>
#include <sqee/misc/Json.hpp>
#include <sqee/vk/Helpers.hpp>
#include <sqee/vk/VulkanContext.hpp>
#include <sqee/vk/VulkanMemory.hpp>

using namespace sq;

//==============================================================================

inline void impl_ascii_append_normal(std::byte*& ptr, StringView svx, StringView svy, StringView svz, StringView svw)
{
    const float fw = svw.empty() ? 0.f : parse_number<float>(svw);
    const float fx = std::round(parse_number<float>(svx) * 511.f);
    const float fy = std::round(parse_number<float>(svy) * 511.f);
    const float fz = std::round(parse_number<float>(svz) * 511.f);

    const uint32_t uw = (int32_t(fw) &   1) << 30 | (fw < 0.f) << 31;
    const uint32_t ux = (int32_t(fx) & 511) << 20 | (fx < 0.f) << 29;
    const uint32_t uy = (int32_t(fy) & 511) << 10 | (fy < 0.f) << 19;
    const uint32_t uz = (int32_t(fz) & 511) <<  0 | (fz < 0.f) <<  9;

    *reinterpret_cast<uint32_t*>(ptr) = uw | ux | uy | uz;
    ptr += sizeof(uint32_t);
}

inline void impl_ascii_append_float(std::byte*& ptr, StringView sv)
{
    *reinterpret_cast<float*>(ptr) = parse_number<float>(sv);
    ptr += sizeof(float);
}

inline void impl_ascii_append_unorm8(std::byte*& ptr, StringView sv)
{
    *reinterpret_cast<uint8_t*>(ptr) = uint8_t(parse_number<float>(sv) * 255.f);
    ptr += sizeof(uint8_t);
}

inline void impl_ascii_append_int8(std::byte*& ptr, StringView sv)
{
    *reinterpret_cast<int8_t*>(ptr) = parse_number<int8_t>(sv);
    ptr += sizeof(int8_t);
}

//==============================================================================

Mesh::Mesh(Mesh&& other)
{
    *this = std::move(other);
}

Mesh& Mesh::operator=(Mesh&& other)
{
    std::swap(mVertexBuffer, other.mVertexBuffer);
    std::swap(mIndexBuffer, other.mIndexBuffer);
    mVertexSize = other.mVertexSize;
    mVertexTotal = other.mVertexTotal;
    mIndexTotal = other.mIndexTotal;
    mAttributes = other.mAttributes;
    mBounds = other.mBounds;
    mSubMeshes = std::move(other.mSubMeshes);
    return *this;
}

Mesh::~Mesh()
{
    if (!mVertexBuffer.buffer) return;
    const auto& ctx = VulkanContext::get();
    mVertexBuffer.destroy(ctx);
    mIndexBuffer.destroy(ctx);
}

//==============================================================================

void Mesh::load_from_file(const String& path)
{
    SQASSERT(!mVertexBuffer.buffer, "mesh already loaded");

    if (auto text = try_read_text_from_file(path + ".sqm"))
        impl_load_text(std::move(*text));

    else if (auto text = try_read_text_from_file(path))
        impl_load_text(std::move(*text));

    else SQEE_THROW("could not find mesh '{}'", path);
}

//==============================================================================

int8_t Mesh::get_sub_mesh_index(TinyString name) const noexcept
{
    for (size_t i = 0u; i < mSubMeshes.size(); ++i)
        if (mSubMeshes[i].name == name)
            return int8_t(i);
    return -1;
}

int8_t Mesh::json_as_sub_mesh_index(JsonAny json) const
{
    if (json.is_null()) return -1;
    if (auto index = get_sub_mesh_index(json.as<TinyString>()); index >= 0) return index;
    json.throw_with_context("invalid sub mesh name");
}

//==============================================================================

void Mesh::bind_buffers(vk::CommandBuffer cmdbuf) const
{
    cmdbuf.bindVertexBuffers(0u, mVertexBuffer.buffer, size_t(0u));
    cmdbuf.bindIndexBuffer(mIndexBuffer.buffer, 0u, vk::IndexType::eUint32);
}

void Mesh::draw(vk::CommandBuffer cmdbuf, int8_t subMesh) const
{
    if (subMesh >= 0)
    {
        SQASSERT(size_t(subMesh) < mSubMeshes.size(), "invalid submesh");
        const SubMesh& sm = mSubMeshes[subMesh];
        cmdbuf.drawIndexed(sm.indexCount, 1u, sm.firstIndex, 0, 0u);
    }
    else cmdbuf.drawIndexed(mIndexTotal, 1u, 0u, 0, 0u);
}

//==============================================================================

void Mesh::impl_load_text(String&& text)
{
    enum class Section { None, Header, Vertices, Indices };
    Section section = Section::None;

    std::vector<std::byte> vertexData;
    std::vector<uint32_t> indexData;

    std::byte* vertexPtr = nullptr;
    uint32_t* indexPtr = nullptr;

    // all meshes will have position data
    mVertexSize = sizeof(float[3]);
    size_t numValuesPerVertex = 3u;

    //----------------------------------------------------------

    for (const auto& [line, num] : TokenisedFile::from_string(std::move(text)).lines)
    {
        const StringView& key = line.front();

        if (key.front() == '#') continue;

        if (key == "SECTION")
        {
            if (line[1] == "Header") section = Section::Header;

            else if (line[1] == "Vertices")
            {
                section = Section::Vertices;
                vertexData.resize(mVertexTotal * mVertexSize);
                vertexPtr = vertexData.data();
            }

            else if (line[1] == "Indices")
            {
                section = Section::Indices;
                indexData.resize(mIndexTotal);
                indexPtr = indexData.data();
            }

            else SQEE_THROW("{}: invalid section", num);
        }

        else if (section == Section::Header)
        {
            if (key == "Attributes")
            {
                for (size_t i = 1u; i < line.size(); ++i)
                {
                    if (line[i] == "TexCoords")
                    {
                        mAttributes |= Attribute::TexCoords;
                        mVertexSize += sizeof(float[2]);
                        numValuesPerVertex += 2u;
                    }
                    else if (line[i] == "Normals")
                    {
                        mAttributes |= Attribute::Normals;
                        mVertexSize += sizeof(uint32_t);
                        numValuesPerVertex += 3u;
                    }
                    else if (line[i] == "Tangents")
                    {
                        mAttributes |= Attribute::Tangents;
                        mVertexSize += sizeof(uint32_t);
                        numValuesPerVertex += 4u;
                    }
                    else if (line[i] == "Colours")
                    {
                        mAttributes |= Attribute::Colours;
                        mVertexSize += sizeof(uint8_t[4]);
                        numValuesPerVertex += 4u;
                    }
                    else if (line[i] == "Bones")
                    {
                        mAttributes |= Attribute::Bones;
                        mVertexSize += sizeof(int8_t[4]) + sizeof(uint8_t[4]);
                        numValuesPerVertex += 8u;
                    }
                    else SQEE_THROW("{}: invalid attribute", num);
                }
            }

            else if (key == "Origin")
                parse_numbers(mBounds.origin, line[1], line[2], line[3]);

            else if (key == "Extents")
                parse_numbers(mBounds.extents, line[1], line[2], line[3]);

            else if (key == "Radius")
                parse_number(mBounds.radius, line[1]);

            else if (key == "SubMesh")
            {
                SubMesh& sm = mSubMeshes.emplace_back();

                sm.name = line[1];
                sm.firstVertex = mVertexTotal;
                parse_number(sm.vertexCount, line[2]);
                sm.firstIndex = mIndexTotal;
                parse_number(sm.indexCount, line[3]);

                mVertexTotal += sm.vertexCount;
                mIndexTotal += sm.indexCount;
            }

            else SQEE_THROW("unknown header key '{}'", key);
        }

        else if (section == Section::Vertices)
        {
            SQASSERT(vertexPtr < vertexData.data() + vertexData.size(), "too many vertices");
            SQASSERT(line.size() == numValuesPerVertex, "wrong number of values");

            size_t index = 0u;

            impl_ascii_append_float(vertexPtr, line[index++]);
            impl_ascii_append_float(vertexPtr, line[index++]);
            impl_ascii_append_float(vertexPtr, line[index++]);

            if (mAttributes & Attribute::TexCoords)
            {
                impl_ascii_append_float(vertexPtr, line[index++]);
                impl_ascii_append_float(vertexPtr, line[index++]);
            }

            if (mAttributes & Attribute::Normals)
            {
                const auto x = line[index++];
                const auto y = line[index++];
                const auto z = line[index++];
                impl_ascii_append_normal(vertexPtr, x, y, z, {});
            }

            if (mAttributes & Attribute::Tangents)
            {
                const auto x = line[index++];
                const auto y = line[index++];
                const auto z = line[index++];
                const auto w = line[index++];
                impl_ascii_append_normal(vertexPtr, x, y, z, w);
            }

            if (mAttributes & Attribute::Colours)
            {
                impl_ascii_append_unorm8(vertexPtr, line[index++]);
                impl_ascii_append_unorm8(vertexPtr, line[index++]);
                impl_ascii_append_unorm8(vertexPtr, line[index++]);
                impl_ascii_append_unorm8(vertexPtr, line[index++]);
            }

            if (mAttributes & Attribute::Bones)
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

        else if (section == Section::Indices)
        {
            SQASSERT(indexPtr < indexData.data() + indexData.size(), "too many indices");
            SQASSERT(line.size() == 3u, "wrong number of values");

            parse_number(*(indexPtr++), line[0]);
            parse_number(*(indexPtr++), line[1]);
            parse_number(*(indexPtr++), line[2]);
        }

        else SQEE_THROW("missing SECTION");
    }

    //----------------------------------------------------------

    SQASSERT(vertexPtr == vertexData.data() + vertexData.size(), "not enough vertices");
    SQASSERT(indexPtr == indexData.data() + indexData.size(), "not enough indices");

    impl_load_final(vertexData, indexData);
}

//==============================================================================

void Mesh::impl_load_final(std::vector<std::byte>& vertexData, std::vector<uint32_t>& indexData)
{
    const auto& ctx = VulkanContext::get();

    const auto setup_buffer = [&ctx](BufferStuff& stuff, vk::BufferUsageFlags usage, void* data, size_t size)
    {
        stuff.initialise(ctx, size, usage | vk::BufferUsageFlagBits::eTransferDst, false);

        auto staging = StagingBuffer(ctx, true, false, size);
        auto cmdbuf = OneTimeCommands(ctx);

        std::memcpy(staging.memory.map(), data, size);
        staging.memory.unmap();

        cmdbuf->copyBuffer(staging.buffer, stuff.buffer, vk::BufferCopy(0u, 0u, size));
    };

    setup_buffer(mVertexBuffer, vk::BufferUsageFlagBits::eVertexBuffer, vertexData.data(), vertexData.size());
    setup_buffer(mIndexBuffer, vk::BufferUsageFlagBits::eIndexBuffer, indexData.data(), indexData.size() * sizeof(uint32_t));
}

//==============================================================================

Mesh::VertexConfig::VertexConfig(vk::Flags<Attribute> flags, vk::Flags<Attribute> ignored)
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

    if (flags & Attribute::TexCoords & (ignored ^ Attribute::TexCoords))
        attributes.push_back({1u, 0u, vk::Format::eR32G32Sfloat, offsetTCRD});

    if (flags & Attribute::Normals & (ignored ^ Attribute::Normals))
        attributes.push_back({2u, 0u, vk::Format::eA2R10G10B10SnormPack32, offsetNORM});

    if (flags & Attribute::Tangents & (ignored ^ Attribute::Tangents))
        attributes.push_back({3u, 0u, vk::Format::eA2R10G10B10SnormPack32, offsetTANG});

    if (flags & Attribute::Colours & (ignored ^ Attribute::Colours))
        attributes.push_back({4u, 0u, vk::Format::eR8G8B8A8Unorm, offsetCOLR});

    if (flags & Attribute::Bones & (ignored ^ Attribute::Bones))
    {
        attributes.push_back({5u, 0u, vk::Format::eR8G8B8A8Sint, offsetBONEI});
        attributes.push_back({6u, 0u, vk::Format::eR8G8B8A8Unorm, offsetBONEW});
    }

    state = vk::PipelineVertexInputStateCreateInfo({}, binding, attributes);
}
