// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/misc/Builtins.hpp>
#include <sqee/maths/Builtins.hpp>

#include <sqee/gl/FixedBuffer.hpp>
#include <sqee/gl/VertexArray.hpp>

namespace sq {

//============================================================================//

/// The SQEE Mesh class.
class SQEE_API Mesh final : private MoveOnly
{
public: //====================================================//

    /// Load the mesh from an SQM file.
    void load_from_file(const String& path, bool swapYZ = false);

    //--------------------------------------------------------//

    /// Check if the mesh has the TexCoord attribute.
    bool has_TCRD() const { return mOptionsBits & 0b10000; }

    /// Check if the mesh has the Normal attribute.
    bool has_NORM() const { return mOptionsBits & 0b01000; }

    /// Check if the mesh has the Tangent attribute.
    bool has_TANG() const { return mOptionsBits & 0b00100; }

    /// Check if the mesh has the Colour attribute.
    bool has_COLR() const { return mOptionsBits & 0b00010; }

    /// Check if the mesh has Bones & Weights attributes.
    bool has_BONE() const { return mOptionsBits & 0b00001; }

    //--------------------------------------------------------//

    /// Access the mesh's vertex array object.
    const VertexArray& get_vao() const { return mVertexArray; }

    /// Get the number of sub-meshes the mesh has.
    uint get_sub_mesh_count() const { return uint(mSubMeshVec.size()); }

    //--------------------------------------------------------//

    /// Access the mesh's bounding box origin.
    const Vec3F& get_origin() const { return mOrigin; }

    /// Access the mesh's bounding box extents.
    const Vec3F& get_extents() const { return mExtents; }

    /// Access the mesh's bounding box radius.
    const float& get_radius() const { return mRadius; }

    //--------------------------------------------------------//

    /// Draw the full mesh.
    void draw_complete() const;

    /// Draw the specified sub-mesh.
    void draw_partial(uint index) const;

    //--------------------------------------------------------//

    /// Create a new sq::Mesh from sqee packages.
    static UniquePtr<Mesh> make_from_package(const String& path);

private: //===================================================//

    uint8_t mOptionsBits = 0b00000;

    bool mSwapYZ = false;

    VertexArray mVertexArray;

    FixedBuffer mVertexBuffer;
    FixedBuffer mIndexBuffer;

    //--------------------------------------------------------//

    uint mVertexSize = 0u;
    uint mVertexTotal = 0u;
    uint mIndexTotal = 0u;

    struct SubMesh
    {
        uint firstVertex, vertexCount;
        uint firstIndex, indexCount;
    };

    Vector<SubMesh> mSubMeshVec;

    //--------------------------------------------------------//

    Vec3F mOrigin = Vec3F();
    Vec3F mExtents = Vec3F();
    float mRadius = 0.f;

    //--------------------------------------------------------//

    void impl_load_ascii(const String& path);

    void impl_load_final(Vector<std::byte>& vertexData, Vector<uint32_t>& indexData);
};

//============================================================================//

} // namespace sq
