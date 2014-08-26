#pragma once

#include <string>
#include <vector>

#include <gl/gl.hpp>
#include <gl/textures.hpp>

namespace sq {

struct Mesh {
    bool load_from_dir(std::string _dirPath, std::string _skin);

    GLuint vao;
    GLuint ibo;
    sq::Texture2D texNorm;
    sq::Texture2D texDiff;
    sq::Texture2D texSpec;

    ushort iCount;
    int ambi = 0;
};

}
