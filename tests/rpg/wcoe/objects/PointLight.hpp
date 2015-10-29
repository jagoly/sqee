#pragma once

#include <sqee/gl/Textures.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Volumes.hpp>

#include "../Object.hpp"
#include "../Animation.hpp"

namespace sqt { namespace wcoe {

class PointLight : public Object {
public:
    PointLight(const string& _name, Cell* _cell);

    void load_from_spec(const ObjSpec& _spec);

    void refresh(), update();
    void calc(double _accum);
    void animate();

    bool  PROP_shadow    = false;
    bool  PROP_specular  = false;
    bool  PROP_probes    = false;
    Vec3F PROP_position  = {0.f, 0.f, 0.f};
    Vec3F PROP_colour    = {1.f, 1.f, 1.f};
    float PROP_intensity = 10.f;
    uint  PROP_texsize   = 0u;

    AnimatorVec3F ANIM_position  {&PROP_position};
    AnimatorVec3F ANIM_colour    {&PROP_colour};
    AnimatorFloat ANIM_intensity {&PROP_intensity};

    sq::TextureCube tex;
    sq::UniformBuffer ubo;
    array<sq::Frustum, 6> frusArr;
    array<Mat4F, 6> matArr;
    sq::Sphere sphere;
    Mat4F modelMat;
};

}}
