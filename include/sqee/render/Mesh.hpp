#pragma once

#include <sqee/builtins.hpp>
#include <sqee/gl/FixedBuffer.hpp>
#include <sqee/gl/VertexArray.hpp>
#include <sqee/maths/Volumes.hpp>

namespace sq {

/// The SQEE Mesh class
class Mesh final : NonCopyable {
public:
    Mesh(const string& _path);

    uint get_vertCount() const { return vertCount; }
    uint get_faceCount() const { return faceCount; }
    uint get_mtrlCount() const { return mtrlCount; }

    bool has_NORM() const { return optionsBits & 0b10000; }
    bool has_TANG() const { return optionsBits & 0b01000; }
    bool has_TCRD() const { return optionsBits & 0b00100; }
    bool has_COLR() const { return optionsBits & 0b00010; }
    bool has_BONE() const { return optionsBits & 0b00001; }

    BoundBox create_BoundBox(const Mat4F& _modelMat) const;

    void draw_complete() const;
    void draw_material(uint _index) const;
    void bind_vao() const { vertArr.bind(); }

private:
    float radius = 0.f;
    Vec3F origin; Vec3F bbsize;
    uint vertCount, faceCount, mtrlCount;
    uchar optionsBits = 0b00000;

    FixedBuffer vertBuf, elemBuf;
    VertexArray vertArr;

    vector<pair<uint, uint>> mtrlVec;
    uint elementTotal = 0u;

    void load_ascii(const string& _path);
    //void load_binary(const string& _path);

    struct VertData {
        vector<Vec3F> points;
        vector<Vec3F> normals;
        vector<Vec4F> tangents;
        vector<Vec2F> texcrds;
        vector<Vec3F> colours;
        vector<Vec4I> bones;
        vector<Vec4F> weights;
    };

    using FaceData = vector<vector<Vec3U>>;

    void load_final(const VertData& _v, const FaceData& _f);
};

}
