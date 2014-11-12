#pragma once

#include "object.hpp"

namespace sqt {
namespace wld {

class Model : public Object {
public:
    using Object::Object;
    void create(ObjectSpec& _spec);

    sq::Mesh* mesh;
    sq::Texture* texNorm;
    sq::Texture* texDiff;
    sq::Texture* texSpec;
    sq::Texture* texAmbi;
    bool alpha, shad, refl, refr;

    glm::mat4 modelMat;

    void bind_textures();
};

}
}
