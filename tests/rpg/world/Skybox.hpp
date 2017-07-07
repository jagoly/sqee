#pragma once

#include <sqee/builtins.hpp>

namespace sqt::world {

//============================================================================//

class Skybox final : sq::NonCopyable
{
public: //====================================================//

    /// Set the cubemap texture to use.
    void set_texture(const string& texture);

    /// Set the shader saturation paramater.
    void set_saturation(float saturation);

    /// Set the shader brightness paramater.
    void set_brightness(float brightness);

    /// Set the shader contrast paramater.
    void set_contrast(float contrast);

    /// Set the shader opacity paramater.
    void set_opacity(float opacity);

    //--------------------------------------------------------//

    /// Get the cubemap texture being used.
    const string& get_texture() const { return mTexture; }

    /// Get the shader saturation paramater.
    float get_saturation() const { return mSaturation; }

    /// Get the shader brightness paramater.
    float get_brightness() const { return mBrightness; }

    /// Get the shader contrast paramater.
    float get_contrast() const { return mContrast; }

    /// Get the shader opacity paramater.
    float get_opacity() const { return mOpacity; }

private: //===================================================//

    string mTexture    = "";
    float  mSaturation = 1.f;
    float  mBrightness = 0.f;
    float  mContrast   = 1.f;
    float  mOpacity    = 1.f;
};

//============================================================================//

} // namespace sqt::world
