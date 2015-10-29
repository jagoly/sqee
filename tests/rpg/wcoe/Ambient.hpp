#pragma once

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/maths/Vectors.hpp>

#include "Animation.hpp"

namespace sqt { namespace wcoe {

class World;

// todo: add invalidate() to match other objects

class Ambient final : sq::NonCopyable {
public:
    Ambient(const World& _world);

    void refresh(), update();
    void calc(double _accum);
    void animate();

    bool  PROP_enabled = false;
    Vec3F PROP_colour  = {0.5f, 0.5f, 0.5f};

    AnimatorVec3F ANIM_colour {&PROP_colour};

    sq::UniformBuffer ubo;

private:
    const World& world;
};

}}
