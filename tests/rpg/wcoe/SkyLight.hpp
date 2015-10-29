#pragma once

#include <sqee/gl/Textures.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Volumes.hpp>

#include "Animation.hpp"

namespace sqt { namespace wcoe {

class World;

// todo: add invalidate() to match other objects

class SkyLight final : sq::NonCopyable {
public:
    SkyLight(const World& _world);

    void refresh(), update();
    void calc(double _accum);
    void animate();

    bool  PROP_enabled   = false;
    Vec3F PROP_direction = {0.f, 0.f, -1.f};
    Vec3F PROP_colour    = {1.f, 1.f, 1.f};
    float PROP_density   = 0.25f;

    AnimatorNormF ANIM_direction {&PROP_direction};
    AnimatorVec3F ANIM_colour    {&PROP_colour};
    AnimatorFloat ANIM_density   {&PROP_density};

    sq::UniformBuffer ubo;
    sq::Texture2DArray texA;
    sq::Texture2DArray texB;
    array<sq::OrthoFrus, 4> orthArrA;
    array<sq::OrthoFrus, 2> orthArrB;
    array<Mat4F, 4> matArrA;
    array<Mat4F, 2> matArrB;
    Vec4F splits;

private:
    const World& world;
};

}}
