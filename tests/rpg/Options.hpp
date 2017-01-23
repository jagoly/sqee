#pragma once

#include <sqee/setup.hpp>
#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sqt {

//============================================================================//

/// SQEE RPG Global Options
class Options : sq::NonCopyable
{
public:

    //========================================================//

    /// The Size of the Viewport
    Vec2U Window_Size = {800u, 600u};

    /// The Maximum Visible Distance
    float View_Distance = 50.f;

    /// Use High Quality Shadow Filtering
    bool Shadows_Filter = true;

    /// Use High Resolution Shadow Textures
    bool Shadows_Large = true;

    /// Enable Simple Vignette Effect
    bool Vignette_Enable = true;

    /// Enable HDR Bloom Effect
    bool Bloom_Enable = true;

    /// Light Shafts Effect Quality
    uchar Shafts_Quality = 2u;

    /// Screen Space Ambient Occlusion Quality
    uchar SSAO_Quality = 2u;

    /// Full Screen Anti-Aliasing Quality
    uchar FSAA_Quality = 2u;

    /// Debug Texture to Draw to Screen
    string Debug_Texture = "";

    //========================================================//

    /// Assert that options are valid
    void validate() const;

    /// Get the Static Instance
    static Options& get();

    //========================================================//

    /// Private Constructor
    private: Options() = default;
};

//============================================================================//

} // namespace sqt
