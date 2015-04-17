#pragma once
#include <sqee/forward.hpp>

#include <sqee/models/mesh.hpp>
#include <sqee/models/skin.hpp>

namespace sq {

class ModelStatic : NonCopyable {
public:
    mat4 matrix;
    BoundBox bbox;
    Mesh* mesh = nullptr;
    Skin* skin = nullptr;

    void setup();
};

}
