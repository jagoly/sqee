#pragma once
#include "forward.hpp"

#include "models/mesh.hpp"
#include "models/skin.hpp"
#include "models/skeleton.hpp"

namespace sq {

class ModelSkelly : NonCopyable {
public:
    mat4 matrix;
    Skeleton skel;

    Mesh* mesh = nullptr;
    Skin* skin = nullptr;

    void setup();
};

}
