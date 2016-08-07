#pragma once

#include <sqee/builtins.hpp>

namespace sqt { namespace world {

struct SkyBoxObject : sq::NonCopyable {

    /// shader saturation paramater
    float PROP_saturation = 1.f;

    /// shader brightness paramater
    float PROP_brightness = 0.f;

    /// shader contrast paramater
    float PROP_contrast = 1.f;

    /// name of the texture used
    string PROP_texture = "";

    /// texture blending amount
    float PROP_opacity = 1.f;
};

}}
