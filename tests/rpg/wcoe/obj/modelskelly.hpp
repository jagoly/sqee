#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class ModelSkelly : public Object {
public:
    ModelSkelly(const string& _name, const Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void update_from_data();

    vec3 DAT_pos = {0.f, 0.f, 0.f};
    vec3 DAT_rot = {0.f, 0.f, 0.f};
    vec3 DAT_sca = {1.f, 1.f, 1.f};
    bool DAT_shadow = false;
    bool DAT_render = false;
    bool DAT_reflect = false;
    bool DAT_refract = false;
    string DAT_mPath;
    string DAT_sPath;

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
