#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/debug/Logging.hpp>

#include <sqee/misc/Resource.hpp>
#include <sqee/misc/Files.hpp>

#include <sqee/render/Mesh.hpp>

using namespace sq;

//============================================================================//

namespace { // anonymous

using GL_Float32 = GLfloat;
using GL_SNorm16 = GLshort;
using GL_UNorm8 = GLubyte;
using GL_SInt8 = GLbyte;

inline void impl_ascii_append_Float32(uchar*& ptr, const string& str)
{
    const auto value = GL_Float32(stof(str));
    reinterpret_cast<GL_Float32&>(*ptr) = value;
    std::advance(ptr, sizeof(GL_Float32));
}

inline void impl_ascii_append_SNorm16(uchar*& ptr, const string& str)
{
    const auto value = GL_SNorm16(stof(str) * 32767.f);
    reinterpret_cast<GL_SNorm16&>(*ptr) = value;
    std::advance(ptr, sizeof(GL_SNorm16));
}

inline void impl_ascii_append_UNorm8(uchar*& ptr, const string& str)
{
    const auto value = GL_UNorm8(stof(str) * 255.f);
    reinterpret_cast<GL_UNorm8&>(*ptr) = value;
    std::advance(ptr, sizeof(GL_UNorm8));
}

inline void impl_ascii_append_SInt8(uchar*& ptr, const string& str)
{
    const auto value = GL_SInt8(stoi(str));
    reinterpret_cast<GL_SInt8&>(*ptr) = value;
    std::advance(ptr, sizeof(GL_SInt8));
}

} // anonymous namespace

//============================================================================//

Mesh::Mesh() : mVertexBuffer(gl::ARRAY_BUFFER), mIndexBuffer(gl::ELEMENT_ARRAY_BUFFER) {}

//============================================================================//

void Mesh::load_from_file(const string& path)
{
    impl_load_ascii("assets/" + path + ".sqm");
}

//============================================================================//

void Mesh::draw_complete() const
{
    gl::DrawElements(gl::TRIANGLES, int(mIndexTotal), gl::UNSIGNED_INT, nullptr);
}

void Mesh::draw_partial(uint index) const
{
    const auto& subMesh = mSubMeshVec[index];
    const auto startLong = long(subMesh.firstIndex * 4u);
    const auto start = reinterpret_cast<void*>(startLong);
    const auto count = GLsizei(subMesh.indexCount);

    gl::DrawElements(gl::TRIANGLES, count, gl::UNSIGNED_INT, start);
}

//============================================================================//

void Mesh::impl_load_ascii(const string& path)
{
    string section = "";

    vector<uchar> vertexData;
    vector<uint> indexData;

    bool allocatedVertices = false;
    bool allocatedIndices = false;

    uchar* vertexPtr = nullptr;
    uint* indexPtr = nullptr;

    //========================================================//

    //for (const auto& [line, num] : tokenise_file(path))
    for (const auto& linePair : tokenise_file(path))
    {
        const auto& line = linePair.first;
        //const auto& num = linePair.second;
        const auto& key = line.front();

        //========================================================//

        if (key.front() == '#') continue;

        else if (key == "{")
        {
            if (section.empty()) section = line[1];
            else log_warning("stray { in mesh '%s'", path);
        }

        else if (key == "}")
        {
            if (!section.empty()) section.clear();
            else log_warning("stray } in mesh '%s'", path);
        }

        //========================================================//

        else if (section == "header")
        {
            if (key == "Attrs")
            {
                for (uint i = 1u; i < line.size(); ++i)
                {
                    if (line[i] == "TCRD") mOptionsBits |= 0b10000;
                    if (line[i] == "NORM") mOptionsBits |= 0b01000;
                    if (line[i] == "TANG") mOptionsBits |= 0b00100;
                    if (line[i] == "COLR") mOptionsBits |= 0b00010;
                    if (line[i] == "BONE") mOptionsBits |= 0b00001;
                }
            }

            else if (key == "Origin") mOrigin = { stof(line[1]), stof(line[2]), stof(line[3]) };

            else if (key == "Extents") mExtents = { stof(line[1]), stof(line[2]), stof(line[3]) };

            else if (key == "Radius") mRadius = stof(line[1]);

            else if (key == "SubMesh")
            {
                const uint firstVertex = mVertexTotal;
                const uint firstIndex = mIndexTotal;

                const uint vertexCount = stou(line[1]);
                const uint indexCount = stou(line[2]);

                mSubMeshVec.push_back({firstVertex, vertexCount, firstIndex, indexCount});

                mVertexTotal += vertexCount;
                mIndexTotal += indexCount;
            }

            else log_warning("invalid header key '%s' in mesh '%s'", key, path);
        }

        //========================================================//

        else if (section == "vertices")
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

        //========================================================//

        else if (section == "indices")
        {
            if (allocatedIndices == false)
            {
                indexData.resize(mIndexTotal);
                indexPtr = indexData.data();
                allocatedIndices = true;
            }

            *(indexPtr++) = stou(line[0]);
            *(indexPtr++) = stou(line[1]);
            *(indexPtr++) = stou(line[2]);
        }

        //========================================================//

        else log_error("invalid section '%s' in mesh '%s'", section, path);
    }

    //========================================================//

    SQASSERT(vertexPtr == vertexData.end().base(), "");
    SQASSERT(indexPtr == indexData.end().base(), "");

    impl_load_final(vertexData, indexData);
}

//============================================================================//

void Mesh::impl_load_final(const vector<uchar>& vertexData, const vector<uint>& indexData)
{
    mVertexBuffer.allocate_constant(vertexData.size(), vertexData.data());
    mIndexBuffer.allocate_constant(indexData.size() * 4u, indexData.data());

    mVAO.set_vertex_buffer(mVertexBuffer, 0u, mVertexSize);
    mVAO.set_index_buffer(mIndexBuffer);

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

    mVAO.add_float_attribute(0u, 3u, gl::FLOAT, false, 0u);

    if (has_TCRD()) mVAO.add_float_attribute(1u, 2u, gl::FLOAT, false, offsetTCRD);
    if (has_NORM()) mVAO.add_float_attribute(2u, 3u, gl::SHORT, true, offsetNORM);
    if (has_TANG()) mVAO.add_float_attribute(3u, 4u, gl::SHORT, true, offsetTANG);
    if (has_COLR()) mVAO.add_float_attribute(4u, 3u, gl::FLOAT, false, offsetCOLR);

    if (has_BONE()) mVAO.add_integer_attribute(5u, 4u, gl::BYTE, offsetBoneI);
    if (has_BONE()) mVAO.add_float_attribute(6u, 4u, gl::UNSIGNED_BYTE, true, offsetBoneW);
}

//============================================================================//

unique_ptr<Mesh> Mesh::make_from_package(const string& path)
{
    const string::size_type splitPos = path.find(':');
    log_assert(splitPos != string::npos, "bad path '%s'", path);

    const string package = path.substr(0u, splitPos);
    const string name = path.substr(splitPos + 1u);

    unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
    mesh->load_from_file("packages/" + package + "/meshes/" + name);

    return mesh;
}
