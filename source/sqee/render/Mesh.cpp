#include <sqee/redist/gl_loader.hpp>

#include <sqee/assert.hpp>
#include <sqee/debug/Logging.hpp>

#include <sqee/misc/Files.hpp>
#include <sqee/misc/Parsing.hpp>
#include <sqee/helpers.hpp>

#include <sqee/render/Mesh.hpp>

using namespace sq;

//============================================================================//

void Mesh::load_from_file(const string& path, bool swapYZ)
{
    mSwapYZ = swapYZ;

    if (check_file_exists(path)) impl_load_ascii(path);
    else if (auto _path = path + ".sqm"; check_file_exists(_path)) impl_load_ascii(_path);
    else log_error("Failed to find mesh '%s'", path);
}

//============================================================================//

void Mesh::draw_complete() const
{
    gl::DrawElements(gl::TRIANGLES, int(mIndexTotal), gl::UNSIGNED_INT, nullptr);
}

void Mesh::draw_partial(uint index) const
{
    const auto& subMesh = mSubMeshVec[index];
    const auto startInt = ptrdiff_t(subMesh.firstIndex * 4u);

    const auto start = reinterpret_cast<GLuint*>(startInt);
    const auto count = GLsizei(subMesh.indexCount);

    gl::DrawElements(gl::TRIANGLES, count, gl::UNSIGNED_INT, start);
}

//============================================================================//

namespace { // anonymous

using GL_Float32 = GLfloat;
using GL_SNorm16 = GLshort;
using GL_UNorm8 = GLubyte;
using GL_SInt8 = GLbyte;

inline void impl_ascii_append_Float32(uchar*& ptr, string_view sv)
{
    const auto value = GL_Float32(sv_to_f(sv));
    reinterpret_cast<GL_Float32&>(*ptr) = value;
    std::advance(ptr, sizeof(GL_Float32));
}

inline void impl_ascii_append_SNorm16(uchar*& ptr, string_view sv)
{
    const auto value = GL_SNorm16(sv_to_f(sv) * 32767.f);
    reinterpret_cast<GL_SNorm16&>(*ptr) = value;
    std::advance(ptr, sizeof(GL_SNorm16));
}

inline void impl_ascii_append_UNorm8(uchar*& ptr, string_view sv)
{
    const auto value = GL_UNorm8(sv_to_f(sv) * 255.f);
    reinterpret_cast<GL_UNorm8&>(*ptr) = value;
    std::advance(ptr, sizeof(GL_UNorm8));
}

inline void impl_ascii_append_SInt8(uchar*& ptr, string_view sv)
{
    const auto value = GL_SInt8(sv_to_i(sv));
    reinterpret_cast<GL_SInt8&>(*ptr) = value;
    std::advance(ptr, sizeof(GL_SInt8));
}

template <class Type>
inline void impl_swap_attr_yz(uchar* vertexPtr, uint attrOffset)
{
    Type* attrPtr = reinterpret_cast<Type*>(vertexPtr + attrOffset);
    std::swap(*std::next(attrPtr, 1), *std::next(attrPtr, 2));
}

} // anonymous namespace

//============================================================================//

void Mesh::impl_load_ascii(const string& path)
{
    enum class Section { None, Header, Vertices, Indices };
    Section section = Section::None;

    std::vector<uchar> vertexData;
    std::vector<uint> indexData;

    bool allocatedVertices = false;
    bool allocatedIndices = false;

    uchar* vertexPtr = nullptr;
    uint* indexPtr = nullptr;

    //========================================================//

    for (const auto& [line, num] : tokenise_file(path).lines)
    {
        const auto& key = line.front();

        //--------------------------------------------------------//

        if (key.front() == '#') continue;

        if (key == "SECTION")
        {
            if      (line[1] == "Header")   section = Section::Header;
            else if (line[1] == "Vertices") section = Section::Vertices;
            else if (line[1] == "Indices")  section = Section::Indices;

            else log_error("invalid section '%s' in mesh '%s'", line[1], path);
        }

        //--------------------------------------------------------//

        else if (section == Section::Header)
        {
            if (key == "Attrs")
            {
                for (uint i = 1u; i < line.size(); ++i)
                {
                    if      (line[i] == "TCRD") mOptionsBits |= 0b10000;
                    else if (line[i] == "NORM") mOptionsBits |= 0b01000;
                    else if (line[i] == "TANG") mOptionsBits |= 0b00100;
                    else if (line[i] == "COLR") mOptionsBits |= 0b00010;
                    else if (line[i] == "BONE") mOptionsBits |= 0b00001;

                    else log_warning("unknown attribute '%s' in mesh '%s'", line[i], path);
                }
            }

            else if (key == "Origin") sq::parse_tokens ( mOrigin, line[1], line[2], line[3] );
            else if (key == "Extents") sq::parse_tokens ( mExtents, line[1], line[2], line[3] );
            else if (key == "Radius") sq::parse_tokens ( mRadius, line[1] );

            else if (key == "SubMesh")
            {
                const uint firstVertex = mVertexTotal;
                const uint firstIndex = mIndexTotal;

                const uint vertexCount = sv_to_u(line[1]);
                const uint indexCount = sv_to_u(line[2]);

                mSubMeshVec.push_back({firstVertex, vertexCount, firstIndex, indexCount});

                mVertexTotal += vertexCount;
                mIndexTotal += indexCount;
            }

            else log_warning("unknown header key '%s' in mesh '%s'", key, path);
        }

        //--------------------------------------------------------//

        else if (section == Section::Vertices)
        {
            if (allocatedVertices == false)
            {
                mVertexSize = sizeof(GL_Float32[3]);
                if (has_TCRD()) mVertexSize += sizeof(GL_Float32[2]);
                if (has_NORM()) mVertexSize += sizeof(GL_SNorm16[3]);
                if (has_TANG()) mVertexSize += sizeof(GL_SNorm16[4]);
                if (has_COLR()) mVertexSize += sizeof(GL_Float32[3]);
                if (has_BONE()) mVertexSize += sizeof(GL_SInt8[4]);
                if (has_BONE()) mVertexSize += sizeof(GL_UNorm8[4]);

                vertexData.resize(mVertexTotal * mVertexSize);
                vertexPtr = vertexData.data();
                allocatedVertices = true;
            }

            uint ind = 0u;

            impl_ascii_append_Float32(vertexPtr, line[ind++]);
            impl_ascii_append_Float32(vertexPtr, line[ind++]);
            impl_ascii_append_Float32(vertexPtr, line[ind++]);

            if (has_TCRD() == true)
            {
                impl_ascii_append_Float32(vertexPtr, line[ind++]);
                impl_ascii_append_Float32(vertexPtr, line[ind++]);
            }

            if (has_NORM() == true)
            {
                impl_ascii_append_SNorm16(vertexPtr, line[ind++]);
                impl_ascii_append_SNorm16(vertexPtr, line[ind++]);
                impl_ascii_append_SNorm16(vertexPtr, line[ind++]);
            }

            if (has_TANG() == true)
            {
                impl_ascii_append_SNorm16(vertexPtr, line[ind++]);
                impl_ascii_append_SNorm16(vertexPtr, line[ind++]);
                impl_ascii_append_SNorm16(vertexPtr, line[ind++]);
                impl_ascii_append_SNorm16(vertexPtr, line[ind++]);
            }

            if (has_COLR() == true)
            {
                impl_ascii_append_Float32(vertexPtr, line[ind++]);
                impl_ascii_append_Float32(vertexPtr, line[ind++]);
                impl_ascii_append_Float32(vertexPtr, line[ind++]);
            }

            if (has_BONE() == true)
            {
                impl_ascii_append_SInt8(vertexPtr, line[ind++]);
                impl_ascii_append_SInt8(vertexPtr, line[ind++]);
                impl_ascii_append_SInt8(vertexPtr, line[ind++]);
                impl_ascii_append_SInt8(vertexPtr, line[ind++]);
            }

            if (has_BONE() == true)
            {
                impl_ascii_append_UNorm8(vertexPtr, line[ind++]);
                impl_ascii_append_UNorm8(vertexPtr, line[ind++]);
                impl_ascii_append_UNorm8(vertexPtr, line[ind++]);
                impl_ascii_append_UNorm8(vertexPtr, line[ind++]);
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

        else log_error("missing SECTION in mesh '%s'", path);
    }

    //========================================================//

    SQASSERT(vertexPtr == std::next(&vertexData.back()), "vertex count mismatch");
    SQASSERT(indexPtr == std::next(&indexData.back()), "index count mismatch");

    impl_load_final(vertexData, indexData);
}

//============================================================================//

void Mesh::impl_load_final(std::vector<uchar>& vertexData, std::vector<uint>& indexData)
{
    const uint sizeTCRD = sizeof(GL_Float32[2]) * has_TCRD();
    const uint sizeNORM = sizeof(GL_SNorm16[3]) * has_NORM();
    const uint sizeTANG = sizeof(GL_SNorm16[4]) * has_TANG();
    const uint sizeCOLR = sizeof(GL_Float32[3]) * has_COLR();
    const uint sizeBONE = sizeof(GL_SInt8[4]) * has_BONE();

    const uint offsetTCRD = sizeof(GL_Float32[3]);
    const uint offsetNORM = offsetTCRD + sizeTCRD;
    const uint offsetTANG = offsetNORM + sizeNORM;
    const uint offsetCOLR = offsetTANG + sizeTANG;

    const uint offsetBoneI = offsetCOLR + sizeCOLR;
    const uint offsetBoneW = offsetBoneI + sizeBONE;

    //--------------------------------------------------------//

    if (mSwapYZ == true)
    {
        for (auto iter = vertexData.begin(); iter != vertexData.end(); iter += mVertexSize)
        {
            impl_swap_attr_yz<GL_Float32>(&*iter, 0u);
            if (has_NORM()) impl_swap_attr_yz<GL_SNorm16>(&*iter, offsetNORM);
            if (has_TANG()) impl_swap_attr_yz<GL_SNorm16>(&*iter, offsetTANG);
        }

        std::swap(mOrigin.y, mOrigin.z);
        std::swap(mExtents.y, mExtents.z);
    }

    //--------------------------------------------------------//

    mVertexBuffer.allocate_constant(uint(vertexData.size()), vertexData.data());
    mIndexBuffer.allocate_constant(uint(indexData.size()) * 4u, indexData.data());

    mVertexArray.set_vertex_buffer(mVertexBuffer, mVertexSize);
    mVertexArray.set_index_buffer(mIndexBuffer);

    mVertexArray.add_float_attribute(0u, 3u, gl::FLOAT, false, 0u);

    if (has_TCRD()) mVertexArray.add_float_attribute(1u, 2u, gl::FLOAT, false, offsetTCRD);
    if (has_NORM()) mVertexArray.add_float_attribute(2u, 3u, gl::SHORT, true, offsetNORM);
    if (has_TANG()) mVertexArray.add_float_attribute(3u, 4u, gl::SHORT, true, offsetTANG);
    if (has_COLR()) mVertexArray.add_float_attribute(4u, 3u, gl::FLOAT, false, offsetCOLR);

    if (has_BONE()) mVertexArray.add_integer_attribute(5u, 4u, gl::BYTE, offsetBoneI);
    if (has_BONE()) mVertexArray.add_float_attribute(6u, 4u, gl::UNSIGNED_BYTE, true, offsetBoneW);
}

//============================================================================//

unique_ptr<Mesh> Mesh::make_from_package(const string& path)
{
    const string::size_type splitPos = path.find(':');
    log_assert(splitPos != string::npos, "bad path '%s'", path);

    const string_view package ( path.c_str(), splitPos );
    const char* const name = path.c_str() + splitPos + 1u;

    unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

    mesh->load_from_file(build_string("assets/packages/", package, "/meshes/", name, ".sqm"));

    return mesh;
}
