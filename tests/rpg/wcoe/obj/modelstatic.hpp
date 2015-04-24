#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class ModelStatic : public Object {
public:
    ModelStatic(const string& _name, const Cell& _cell);
    void load_from_spec(const ObjSpec& _spec);

    sq::Mesh* mesh = nullptr;
    sq::Skin* skin = nullptr;
    bool shadow = false, render = false,
        reflect = false, refract = false;

    mat4 matrix;
    sq::BoundBox bbox;
    bool negScale;
};

template<> struct ObjTraits<ModelStatic> {
    static constexpr ObjType type() { return ObjType::ModelStatic; }
};

}}
