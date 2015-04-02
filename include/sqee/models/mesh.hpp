#pragma once
#include <sqee/forward.hpp>

#include <vector>

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
    std::vector<std::pair<GLuint, uint>> iboVec;

    void bind_vao();
    void draw_ibo(uint _mtrl);

    vec3 minPos, maxPos;

private:
    void load_ascii(const string& _path);
    void load_binary(const string& _path);
    void load_final(const std::vector<vec3>& points, const std::vector<vec3>& normals,
                    const std::vector<vec3>& tangents, const std::vector<vec2>& texcrds,
                    const std::vector<ivec4>& bonesA, const std::vector<ivec4>& bonesB,
                    const std::vector<vec4>& weightsA, const std::vector<vec4>& weightsB,
                    const std::vector<std::vector<uvec3>>& faceVec);
};

namespace res {
ResHolder<Mesh>& mesh();
string& mesh_path();
}

}
