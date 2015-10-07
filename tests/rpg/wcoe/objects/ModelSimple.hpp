#pragma once

#include "../Object.hpp"
#include "../Animation.hpp"

namespace sqt { namespace wcoe {

class ModelSimple : public Object {
public:
    ModelSimple(const string& _name, Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void refresh(); void tick();
    void calc(double _accum);

    bool   PROP_render   = false;
    bool   PROP_shadow   = false;
    bool   PROP_decals   = false;
    bool   PROP_probes   = false;
    fvec3  PROP_position = {0.f, 0.f, 0.f};
    fquat  PROP_rotation = {1.f, 0.f, 0.f, 0.f};
    fvec3  PROP_scale    = {1.f, 1.f, 1.f};
    string PROP_mesh     = "";
    string PROP_skin     = "";

    AnimatorFVec3 ANIM_position {&PROP_position};
    AnimatorFQuat ANIM_rotation {&PROP_rotation};
    AnimatorFVec3 ANIM_scale    {&PROP_scale};
    void animate();

    unique_ptr<sq::UniformBuffer> ubo;
    sq::Mesh* mesh = nullptr;
    sq::Skin* skin = nullptr;
    sq::BoundBox bbox;
    bool negScale;
    fmat4 matrix;
};

}}
