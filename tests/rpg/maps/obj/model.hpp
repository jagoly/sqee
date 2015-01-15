#pragma once

#include <sqee/models/mesh.hpp>
#include <sqee/models/skin.hpp>

#include "object.hpp"

namespace sqt {

class Model : public Object {
public:
    Model(const ObjectSpec& _spec);

    glm::mat4 modelMat;

    sq::Mesh* mesh;
    sq::Skin skin;
    bool shad, pnch, refl, refr;    
};

}
