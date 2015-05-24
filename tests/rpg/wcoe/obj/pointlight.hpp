#pragma once

#include "object.hpp"
#include "animation.hpp"

namespace sqt { namespace wcoe {

class PointLight : public Object {
public:
    PointLight(const string& _name, Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void refresh(); void tick();
    void calc(double _accum);

    fvec3 PROP_position  = {0.f, 0.f, 0.f};
    fvec3 PROP_colour    = {1.f, 1.f, 1.f};
    float PROP_intensity = 10.f;
    uint  PROP_texsize   = 0u;
    bool  PROP_shadow    = false;
    bool  PROP_specular  = false;

    AnimatorFVec3 ANIM_position  {&PROP_position};
    AnimatorFVec3 ANIM_colour    {&PROP_colour};
    AnimatorFloat ANIM_intensity {&PROP_intensity};
    void animate();

    array<fmat4, 6> matArr;
    unique_ptr<sq::Uniformbuffer> ubo;
    unique_ptr<sq::TextureCube> tex;
    array<unique_ptr<sq::Framebuffer>, 6> fboArr;
    array<sq::Frustum, 6> frusArr;
    sq::Sphere sphere;
    fmat4 modelMat;
};

template<> struct ObjTraits<PointLight> {
    static constexpr ObjType type() { return ObjType::PointLight; }
};

}}
