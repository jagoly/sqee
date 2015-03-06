#pragma once

#include "object.hpp"

namespace sqt {

class Model : public Object {
public:
    Model(const ObjSpec& _spec);

    glm::mat4 modelMat;

    sq::Mesh* mesh;
    sq::Skin* skin;

    bool shad, pnch, refl, refr;
};

template<> struct ObjTraits<Model> {
    static constexpr ObjType type() { return ObjType::Model; }
};

}
