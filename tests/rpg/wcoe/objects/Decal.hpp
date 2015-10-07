#pragma once

#include "../Object.hpp"
#include "../Animation.hpp"

namespace sqt { namespace wcoe {

class Decal : public Object {
public:
    Decal(const string& _name, Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void refresh(); void tick();
    void calc(double _accum);

    fvec3  PROP_position = {0.f, 0.f, 0.f};
    fquat  PROP_rotation = {1.f, 0.f, 0.f, 0.f};
    fvec3  PROP_scale    = {1.f, 1.f, 1.f};
    float  PROP_alpha    = 1.f;
    string PROP_diff     = "";
    string PROP_norm     = "";
    string PROP_spec     = "";

    AnimatorFVec3 ANIM_position {&PROP_position};
    AnimatorFQuat ANIM_rotation {&PROP_rotation};
    AnimatorFVec3 ANIM_scale    {&PROP_scale};
    AnimatorFloat ANIM_alpha    {&PROP_alpha};
    void animate();

    unique_ptr<sq::UniformBuffer> ubo;
    sq::Texture2D* texDiff = nullptr;
    sq::Texture2D* texNorm = nullptr;
    sq::Texture2D* texSpec = nullptr;
    fmat4 matrix, invMat;
    sq::BoundBox bbox;
};

}}
