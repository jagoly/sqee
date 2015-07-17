#pragma once
#include <sqee/forward.hpp>

#include <sqee/misc/ResHolder.hpp>

namespace sq {

class Mesh : NonCopyable {
public:
    Mesh() = default; ~Mesh();
    Mesh(const string& _path);
    void create(const string& _path);

    fvec3 origin;
    float radius;
    fvec3 bbsize;
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

    GLuint vao, vbo;
    vector<pair<GLuint, uint>> iboVec;

    void bind_vao() const;
    void draw_ibo(uint _mtrl) const;

private:
    struct VertData {
        vector<fvec3> points;
        vector<fvec3> normals;
        vector<fvec4> tangents;
        vector<fvec2> texcrds;
        vector<fvec3> colours;
        vector<ivec4> bonesA;
        vector<ivec4> bonesB;
        vector<fvec4> weightsA;
        vector<fvec4> weightsB;
    };

    using FaceData = vector<vector<uvec3>>;

    void load_ascii(const string& _path);
    void load_binary(const string& _path);
    void load_final(const VertData& _v, const FaceData& _f);
};

namespace res {
ResHolder<Mesh>& mesh();
}

}
