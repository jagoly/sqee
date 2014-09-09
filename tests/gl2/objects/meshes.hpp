#pragma once

#include <string>
#include <vector>
#include <memory>

#include <libsqee/gl/gl.hpp>

#define MODELS_DIR "res/models/"

namespace sqt {

struct Mesh {
    Mesh(std::string& _filePath);
    //~Mesh();

    GLuint vao;
    GLuint ibo;
    ushort iCount;
};
typedef std::shared_ptr<Mesh> MeshPtr;

}
