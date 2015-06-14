#pragma once
#include <sqee/forward.hpp>

#include <sqee/misc/resholder.hpp>

namespace sq {

class Mesh : NonCopyable {
public:
    void create(const string& _path);
    ~Mesh();

    bool hasNM = false;
    bool hasUV = false;
    bool hasBW = false;
    bool hasMT = false;
    uint vCount = 0u;
    uint fCount = 0u;
    uint mCount = 0u;

    GLuint vao;
    GLuint vboP, vboN, vboT, vboTc, vboBa, vboBb, vboWa, vboWb;
    vector<pair<GLuint, uint>> iboVec;

    void bind_vao();
    void draw_ibo(uint _mtrl);

    fvec3 origin, size;
    float radius;

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
