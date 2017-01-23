#pragma once

#include <sqee/builtins.hpp>

#include <sqee/gl/FixedBuffer.hpp>
#include <sqee/gl/VertexArray.hpp>

#include <sqee/maths/Vectors.hpp>

namespace sq {

//============================================================================//

/// The SQEE Mesh Class
class Mesh final : public MoveOnly
{
public:

    //========================================================//

    /// Constructor
    Mesh();

    //========================================================//

    /// Load mesh from a loose sqm file
    void load_from_file(const string& path);

    //========================================================//

    uint get_sub_mesh_count() const { return mSubMeshVec.size(); }

    //========================================================//

    bool has_TCRD() const { return mOptionsBits & 0b10000; }
    bool has_NORM() const { return mOptionsBits & 0b01000; }
    bool has_TANG() const { return mOptionsBits & 0b00100; }
    bool has_COLR() const { return mOptionsBits & 0b00010; }
    bool has_BONE() const { return mOptionsBits & 0b00001; }

    //========================================================//

    void draw_complete() const;
    void draw_partial(uint index) const;

    //========================================================//

    const VertexArray& get_vao() const { return mVAO; }

    //========================================================//

    const Vec3F& get_origin() const { return mOrigin; }
    const Vec3F& get_extents() const { return mExtents; }
    const float& get_radius() const { return mRadius; }

    //========================================================//

    /// Load an sq::Mesh from sqee packages
    static unique_ptr<Mesh> make_from_package(const string& path);

private:

    //========================================================//

    Vec3F mOrigin, mExtents;
    float mRadius = 0.f;

    uchar mOptionsBits = 0b00000;

    FixedBuffer mVertexBuffer;
    FixedBuffer mIndexBuffer;
    VertexArray mVAO;

    struct SubMesh
    {
        uint firstVertex, vertexCount;
        uint firstIndex, indexCount;
    };

    vector<SubMesh> mSubMeshVec;

    uint mVertexSize = 0u;
    uint mVertexTotal = 0u;
    uint mIndexTotal = 0u;

    //========================================================//

    void impl_load_ascii(const string& path);

    void impl_load_final(const vector<uchar>& vertexData, const vector<uint>& indexData);
};

//============================================================================//

} // namespace sq
