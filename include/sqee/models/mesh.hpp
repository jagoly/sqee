#pragma once
#include "forward.hpp"

#include <array>
#include <vector>

namespace sq {

class Mesh : NonCopyable {
public:
    struct Vertex {
        float x=0, y=0, z=0;
        float nx=0, ny=0, nz=0;
        float u=0, v=0;
        int b[8] {-1}; float w[8] {0};
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
    std::vector<std::pair<GLuint, uint>> iboVec;

    std::vector<Vertex> vertVec;
    std::vector<std::vector<std::array<uint, 3>>> mtrlVec;

    void bind_vao();
    void draw_ibo(uint _mtrl = 0);
};


}
