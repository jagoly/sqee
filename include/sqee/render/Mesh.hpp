#pragma once
#include <sqee/forward.hpp>

#include <sqee/misc/ResHolder.hpp>

namespace sq {

class Mesh : NonCopyable {
public:
    void create(const string& _path);
    ~Mesh();

    fvec3 origin;
    float radius;
    fvec3 bbsize;
    bool hasBBox = false;
    bool hasNorm = false;
    bool hasTcrd = false;
    bool hasBone = false;
    bool hasMtrl = false;
    uint vertCount = 0u;
    uint faceCount = 0u;
    uint mtrlCount = 0u;

    GLuint vao;
    GLuint vboP, vboN, vboT, vboTc, vboBa, vboBb, vboWa, vboWb;
    vector<pair<GLuint, uint>> iboVec;

    void bind_vao();
    void draw_ibo(uint _mtrl);


private:
    void load_ascii(const string& _path);
    void load_binary(const string& _path);
    void load_final(const vector<fvec3>& points,   const vector<fvec3>& normals,
                    const vector<fvec3>& tangents, const vector<fvec2>& texcrds,
                    const vector<ivec4>& bonesA,   const vector<ivec4>& bonesB,
                    const vector<fvec4>& weightsA, const vector<fvec4>& weightsB,
                    const vector<vector<uvec3>>& faceVec);
};

namespace res {
ResHolder<Mesh>& mesh();
string& mesh_path();
}

}
