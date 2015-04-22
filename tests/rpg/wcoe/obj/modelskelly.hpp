#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class ModelSkelly : public Object {
public:
    ModelSkelly(const string& _name, const Cell& _cell);
    void load_from_spec(const ObjSpec& _spec);

    sq::Mesh* mesh = nullptr;
    sq::Skin* skin = nullptr;
    unique_ptr<sq::Skeleton> skel;
    bool refl, refr;

    mat4 matrix;
    sq::BoundBox bbox;
    bool negScale;

};

template<> struct ObjTraits<ModelSkelly> {
    static constexpr ObjType type() { return ObjType::ModelSkelly; }
};

}}
