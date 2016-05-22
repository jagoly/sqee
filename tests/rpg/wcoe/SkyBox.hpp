#pragma once

#include <sqee/gl/Textures.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sqt {

class SkyBox final : sq::NonCopyable {
public:
    SkyBox();

    void configure();
    void update();

    bool   PROP_enabled    = false;
    float  PROP_saturation = 1.f;
    float  PROP_brightness = 0.f;
    float  PROP_contrast   = 1.f;
    float  PROP_alpha      = 1.f;
    string PROP_texture    = "";

    sq::TextureCube tex;
    sq::UniformBuffer ubo;
};

}
