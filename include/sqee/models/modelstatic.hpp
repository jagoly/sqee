#pragma once
#include "forward.hpp"

#include "models/mesh.hpp"
#include "models/skin.hpp"

namespace sq {

class ModelStatic : NonCopyable {
public:
    mat4 matrix;

    Mesh* mesh = nullptr;
    Skin* skin = nullptr;

    void setup();
};

}
