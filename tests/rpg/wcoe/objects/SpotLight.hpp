#pragma once

#include "../Object.hpp"
#include "../Animation.hpp"

namespace sqt { namespace wcoe {

class SpotLight : public Object {
public:
    SpotLight(const string& _name, Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void refresh(); void tick();
    void calc(double _accum);

    bool  PROP_shadow    = false;
    bool  PROP_specular  = false;
    bool  PROP_probes    = false;
    fvec3 PROP_direction = {0.f, 0.f, -1.f};
    fvec3 PROP_position  = {0.f, 0.f, 0.f};
    fvec3 PROP_colour    = {1.f, 1.f, 1.f};
    float PROP_intensity = 10.f;
    float PROP_softness  = 0.f;
    float PROP_angle     = 45.f;
    uint  PROP_texsize   = 0u;

    AnimatorFNorm ANIM_direction {&PROP_direction};
    AnimatorFVec3 ANIM_position  {&PROP_position};
    AnimatorFVec3 ANIM_colour    {&PROP_colour};
    AnimatorFloat ANIM_intensity {&PROP_intensity};
    AnimatorFloat ANIM_softness  {&PROP_softness};
    AnimatorFloat ANIM_angle     {&PROP_angle};
    void animate();

    unique_ptr<sq::UniformBuffer> ubo;
    unique_ptr<sq::Texture2D> tex;
    sq::Frustum frus;
    fmat4 modelMat;
    fmat4 matrix;
};

}}
