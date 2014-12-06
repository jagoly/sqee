#pragma once

#include <gl/gl.hpp>
#include <misc/containers.hpp>

namespace sq {

class Mesh {
public:
    struct Vertex {
        Vertex(float _x, float _y, float _z, float _nx, float _ny, float _nz,
               float _u, float _v, array<int, 8> _b, array<float, 8> _w)
            : x(_x), y(_y), z(_z), nx(_nx), ny(_ny), nz(_nz), u(_u), v(_v), b(_b), w(_w) {}
        const float x, y, z, nx, ny, nz, u, v;
        const array<int, 8> b;
        const array<float, 8> w;
    };

    ~Mesh();

    void load(const string& _filePath);

    bool hasNM = false;
    bool hasUV = false;
    bool hasBW = false;
    bool hasMT = false;
    uint vCount = 0;
    uint fCount = 0;

    GLuint vboP, vboN, vboT, vboB, vboW, vboTc;

    GLuint vao;
    vector<pair<GLuint, uint>> iboVec;

    vector<Vertex> vertVec;
    vector<vector<std::array<uint, 3>>> faceVec;

    void bind_vao();
    void draw_ibo(uint _mtrl = 0);
};
typedef ResHolder<string, Mesh> MeshHolder;


}
