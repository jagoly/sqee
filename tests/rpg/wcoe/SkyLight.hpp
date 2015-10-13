#pragma once
#include <sqee/forward.hpp>

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/Textures.hpp>

#include "Animation.hpp"

namespace sqt { namespace wcoe {

class World;

// todo: add invalidate() to match other objects

class SkyLight final : NonCopyable {
public:
    SkyLight(const World& _world);

    void refresh(), update();
    void calc(double _accum);
    void animate();

    bool  PROP_enabled   = false;
    fvec3 PROP_direction = {0.f, 0.f, -1.f};
    fvec3 PROP_colour    = {1.f, 1.f, 1.f};
    float PROP_density   = 0.25f;

    AnimatorFNorm ANIM_direction {&PROP_direction};
    AnimatorFVec3 ANIM_colour    {&PROP_colour};
    AnimatorFloat ANIM_density   {&PROP_density};

    sq::UniformBuffer ubo;
    sq::Texture2DArray texA;
    sq::Texture2DArray texB;
    array<sq::OrthoFrus, 4> orthArrA;
    array<sq::OrthoFrus, 2> orthArrB;
    array<fmat4, 4> matArrA;
    array<fmat4, 2> matArrB;
    fvec4 splits;

private:
    const World& world;
};

}}
