#pragma once

#include <sqee/builtins.hpp>
#include <sqee/gl/FixedBuffer.hpp>
#include <sqee/gl/VertexArray.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sq {

/// The SQEE Mesh class
class Mesh final : NonCopyable {
public:
    Mesh(); ~Mesh();
    Mesh(const string& _path);
    void create(const string& _path);

    float radius = 0.f;
    Vec3F bbsize = {0.f, 0.f, 0.f};
    Vec3F origin = {0.f, 0.f, 0.f};
    bool hasBBox = false;
    bool hasNorm = false;
    bool hasTang = false;
    bool hasTcrd = false;
    bool hasColr = false;
    bool hasBone = false;
    bool hasMtrl = false;
    uint vertCount = 0u;
    uint faceCount = 0u;
    uint mtrlCount = 0u;

    void bind_vao() const;
    void draw_complete() const;
    void draw_material(uint _mtrl) const;

private:
    FixedBuffer vertBuf;
    FixedBuffer elemBuf;
    VertexArray vertArr;
    vector<pair<uint, uint>> mtrlVec;
    uint elementTotal = 0u;

    void load_ascii(const string& _path);
    void load_binary(const string& _path);

    struct VertData {
        vector<Vec2F> texcrds;
        vector<Vec3F> points, normals, colours;
        vector<Vec4F> tangents, weightsA, weightsB;
        vector<Vec4I> bonesA, bonesB;
    };

    using FaceData = vector<vector<Vec3U>>;

    void load_final(const VertData& _v, const FaceData& _f);
};

}
