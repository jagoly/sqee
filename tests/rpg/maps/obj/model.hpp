#pragma once

#include "object.hpp"

namespace sqt {

class Model : public Object {
public:
    Model(const ObjectSpec& _spec);

    glm::mat4 modelMat;

    sq::Mesh* mesh;
    sq::Skin* skin;

    bool shad, pnch, refl, refr;    
};

}
