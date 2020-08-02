#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

using namespace sq::coretypes;

//============================================================================//

namespace sqt {

class Options final : sq::NonCopyable
{
public: //====================================================//

    Options() = default;

    //--------------------------------------------------------//

    /// The Size of the Viewport.
    Vec2U Window_Size = {800u, 600u};

    /// The Maximum Visible Distance.
    float View_Distance = 40.f;

    /// Use High Quality Shadow Filtering.
    bool Shadows_Filter = true;

    /// Use High Resolution Shadow Textures.
    bool Shadows_Large = true;

    /// Enable Simple Vignette Effect.
    bool Vignette_Enable = true;

    /// Enable HDR Bloom Effect.
    bool Bloom_Enable = true;

    /// Light Shafts Effect Quality.
    uchar Shafts_Quality = 2u;

    /// Screen Space Ambient Occlusion Quality.
    uchar SSAO_Quality = 2u;

    /// Full Screen Anti-Aliasing Quality.
    uchar FSAA_Quality = 2u;

    /// Debug Texture to Draw to Screen.
    std::string Debug_Texture = "";

    //--------------------------------------------------------//

    /// Assert that options are valid.
    void validate() const;
};

} // namespace sqt
