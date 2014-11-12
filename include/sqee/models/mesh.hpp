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

    struct Face {
        Face(uint _a, uint _b, uint _c, uchar _m)
            : a(_a), b(_b), c(_c), m(_m) {}
        const uint a, b, c; const uchar m;
    };

    ~Mesh();

    void load(const string& _filePath);

    bool hasUV = false, hasBW = false, hasMID = false;

    GLuint vboP, vboN, vboB, vboW, vboTc;

    GLuint vao;
    GLuint ibo, tboTn, vboTn;
    uint iCount;

    vector<Vertex> vertVec;
    vector<Face> faceVec;

    void bind_buffers(GLenum _texBufSlot = gl::NONE);
};
typedef ResHolder<string, Mesh> MeshHolder;


}
