#pragma once
#include <sqee/forward.hpp>

#include <sqee/misc/ResHolder.hpp>

namespace sq {

/// The SQEE Mesh class
class Mesh final : NonCopyable {
public:
    Mesh() = default; ~Mesh();
    Mesh(const string& _path);
    void create(const string& _path);

    float radius = 0.f;
    fvec3 bbsize = fvec3(0.f);
    fvec3 origin = fvec3(0.f);
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
    unique_ptr<FixedBuffer> vertBuf;
    unique_ptr<FixedBuffer> elemBuf;
    unique_ptr<VertexArray> vertArr;
    vector<pair<size_t, uint>> mtrlVec;
    uint elementTotal = 0u;

    void load_ascii(const string& _path);
    void load_binary(const string& _path);

    struct VertData { vector<fvec2> texcrds;
        vector<fvec3> points, normals, colours;
        vector<fvec4> tangents, weightsA, weightsB;
        vector<ivec4> bonesA, bonesB; };
    using FaceData = vector<vector<uvec3>>;

    void load_final(const VertData& _v, const FaceData& _f);
};

namespace res {
ResHolder<Mesh>& mesh();
}

}
