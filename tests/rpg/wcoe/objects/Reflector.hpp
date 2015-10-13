#pragma once

#include <sqee/gl/UniformBuffer.hpp>

#include "../Object.hpp"
#include "../Animation.hpp"

namespace sqt { namespace wcoe {

class Reflector : public Object {
public:
    Reflector(const string& _name, Cell* _cell);

    void load_from_spec(const ObjSpec& _spec);

    void refresh(), update();
    void calc(double _accum);
    void animate();

    bool   PROP_shadow   = false;
    bool   PROP_decals   = false;
    bool   PROP_probes   = false;
    fvec3  PROP_position = {0.f, 0.f, 0.f};
    fquat  PROP_rotation = {1.f, 0.f, 0.f, 0.f};
    fvec3  PROP_scale    = {1.f, 1.f, 1.f};
    float  PROP_factor   = 1.f;
    string PROP_mesh     = "";
    string PROP_skin     = "";

    AnimatorFVec3 ANIM_position {&PROP_position};
    AnimatorFQuat ANIM_rotation {&PROP_rotation};
    AnimatorFVec3 ANIM_scale    {&PROP_scale};
    AnimatorFloat ANIM_factor   {&PROP_factor};

    sq::Mesh* mesh = nullptr;
    sq::Skin* skin = nullptr;
    sq::UniformBuffer ubo;
    fvec3 trans, normal;
    sq::BoundBox bbox;
    sq::Frustum frus;
    bool negScale;
    fmat4 matrix;
    float offset;
};

}}
