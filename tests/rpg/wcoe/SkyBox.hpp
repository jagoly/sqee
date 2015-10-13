#pragma once
#include <sqee/forward.hpp>

#include <sqee/gl/UniformBuffer.hpp>

#include "Animation.hpp"

namespace sqt { namespace wcoe {

class World;

// todo: add invalidate() to match other objects

class SkyBox final : NonCopyable {
public:
    SkyBox(const World& _camera);

    void refresh(), update();
    void calc(double _accum);
    void animate();

    bool   PROP_enabled    = false;
    float  PROP_saturation = 1.f;
    float  PROP_brightness = 0.f;
    float  PROP_contrast   = 1.f;
    float  PROP_alpha      = 1.f;
    string PROP_texture    = "";

    AnimatorFloat ANIM_saturation {&PROP_saturation};
    AnimatorFloat ANIM_brightness {&PROP_brightness};
    AnimatorFloat ANIM_contrast   {&PROP_contrast};
    AnimatorFloat ANIM_alpha      {&PROP_alpha};

    sq::UniformBuffer ubo;
    sq::TextureCube* tex = nullptr;

private:
    const World& world;
};

}}
