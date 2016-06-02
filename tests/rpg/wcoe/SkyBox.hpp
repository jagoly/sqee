#pragma once

#include <sqee/gl/Textures.hpp>
#include <sqee/gl/UniformBuffer.hpp>

namespace sqt {

class World;

class SkyBox final : sq::NonCopyable {
public:
    SkyBox();

    void configure(const World& _world);
    void update(const World& _world);

    // User Properties /////
    float  PROP_saturation = 1.f;
    float  PROP_brightness = 0.f;
    float  PROP_contrast   = 1.f;
    float  PROP_alpha      = 1.f;
    string PROP_texture    = "";

    // System Properties /////
    string currentTex = "";
    sq::UniformBuffer ubo;
    sq::TextureCube tex;
};

}
