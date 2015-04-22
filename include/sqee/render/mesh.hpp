#pragma once
#include <sqee/forward.hpp>

#include <sqee/gl/maths.hpp>
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
    uint vCount = 0;
    uint fCount = 0;
    uint mCount = 0;

    GLuint vao;
    GLuint vboP, vboN, vboT, vboTc, vboBa, vboBb, vboWa, vboWb;
    vector<std::pair<GLuint, uint>> iboVec;

    void bind_vao();
    void draw_ibo(uint _mtrl);

    BoundBox bbox;

private:
    void load_ascii(const string& _path);
    void load_binary(const string& _path);
    void load_final(const vector<vec3>& points, const vector<vec3>& normals,
                    const vector<vec3>& tangents, const vector<vec2>& texcrds,
                    const vector<ivec4>& bonesA, const vector<ivec4>& bonesB,
                    const vector<vec4>& weightsA, const vector<vec4>& weightsB,
                    const vector<vector<uvec3>>& faceVec);
};

namespace res {
ResHolder<Mesh>& mesh();
string& mesh_path();
}

}
