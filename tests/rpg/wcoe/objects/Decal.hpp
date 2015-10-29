#pragma once

#include <sqee/forward.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Quaternion.hpp>
#include <sqee/maths/Volumes.hpp>

#include "../Object.hpp"
#include "../Animation.hpp"

namespace sqt { namespace wcoe {

class Decal final : public Object {
public:
    Decal(const string& _name, Cell* _cell);

    void load_from_spec(const ObjSpec& _spec);

    void refresh(), update();
    void calc(double _accum);
    void animate();

    Vec3F  PROP_position = {0.f, 0.f, 0.f};
    QuatF  PROP_rotation = {1.f, 0.f, 0.f, 0.f};
    Vec3F  PROP_scale    = {1.f, 1.f, 1.f};
    float  PROP_alpha    = 1.f;
    string PROP_diff     = "";
    string PROP_norm     = "";
    string PROP_spec     = "";

    AnimatorVec3F ANIM_position {&PROP_position};
    AnimatorQuatF ANIM_rotation {&PROP_rotation};
    AnimatorVec3F ANIM_scale    {&PROP_scale};
    AnimatorFloat ANIM_alpha    {&PROP_alpha};

    sq::Texture2D* texDiff = nullptr;
    sq::Texture2D* texNorm = nullptr;
    sq::Texture2D* texSpec = nullptr;
    sq::UniformBuffer ubo;
    Mat4F matrix, invMat;
    sq::BoundBox bbox;
};

}}
