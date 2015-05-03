#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class ModelSkelly : public Object {
public:
    ModelSkelly(const string& _name, const Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void update_from_data();

    struct {
        vec3 pos = {0.f, 0.f, 0.f};
        vec3 rot = {0.f, 0.f, 0.f};
        vec3 sca = {1.f, 1.f, 1.f};
        bool shadow = false;
        bool render = false;
        bool reflect = false;
        bool refract = false;
    } DATA;

    sq::Mesh* mesh = nullptr;
    sq::Skin* skin = nullptr;
    unique_ptr<sq::Skeleton> skel;
    mat4 matrix;
    bool negScale;
    sq::BoundBox bbox;

};

template<> struct ObjTraits<ModelSkelly> {
    static constexpr ObjType type() { return ObjType::ModelSkelly; }
};

}}
