#pragma once
#include <sqee/forward.hpp>

#include <sqee/models/mesh.hpp>
#include <sqee/models/skin.hpp>
#include <sqee/models/skeleton.hpp>

namespace sq {

class ModelSkelly : NonCopyable {
public:
    mat4 matrix;
    Mesh* mesh = nullptr;
    Skin* skin = nullptr;
    Skeleton skel;

    void setup();
};

}