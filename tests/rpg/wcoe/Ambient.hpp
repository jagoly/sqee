#pragma once
#include <sqee/forward.hpp>

#include <sqee/gl/UniformBuffer.hpp>

#include "Animation.hpp"

namespace sqt { namespace wcoe {

class World;

// todo: add invalidate() to match other objects

class Ambient final : NonCopyable {
public:
    Ambient(const World& _world);

    void refresh(), update();
    void calc(double _accum);
    void animate();

    bool  PROP_enabled = false;
    fvec3 PROP_colour  = {0.5f, 0.5f, 0.5f};

    AnimatorFVec3 ANIM_colour {&PROP_colour};

    sq::UniformBuffer ubo;

private:
    const World& world;
};

}}
