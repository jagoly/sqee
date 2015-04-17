#pragma once

#include "object.hpp"

namespace sqt {

class Model : public Object {
public:
    Model(const ObjSpec& _spec);

    shared_ptr<sq::ModelStatic> model;

    bool refl, refr;
};

template<> struct ObjTraits<Model> {
    static constexpr ObjType type() { return ObjType::Model; }
};

}
