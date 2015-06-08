#pragma once

#include "object.hpp"
#include "animation.hpp"

namespace sqt { namespace wcoe {

class ModelStatic : public Object {
public:
    ModelStatic(const string& _name, Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void refresh(); void tick();
    void calc(double _accum);

    bool   PROP_reflect  = false;
    bool   PROP_refract  = false;
    bool   PROP_shadow   = false;
    bool   PROP_render   = false;
    fvec3  PROP_position = {0.f, 0.f, 0.f};
    fquat  PROP_rotation = {1.f, 0.f, 0.f, 0.f};
    fvec3  PROP_scale    = {1.f, 1.f, 1.f};
    string PROP_mPath    = "";
    string PROP_sPath    = "";

    AnimatorFVec3 ANIM_position {&PROP_position};
    AnimatorFQuat ANIM_rotation {&PROP_rotation};
    AnimatorFVec3 ANIM_scale    {&PROP_scale};
    void animate();

    unique_ptr<sq::Uniformbuffer> ubo;
    sq::Mesh* mesh = nullptr;
    sq::Skin* skin = nullptr;
    sq::BoundBox bbox;
    bool negScale;
    fmat4 matrix;
};

template<> struct ObjTraits<ModelStatic> {
    static constexpr ObjType type() { return ObjType::ModelStatic; }
};

}}
