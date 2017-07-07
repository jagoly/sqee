#include "Skybox.hpp"

using namespace sqt::world;

//============================================================================//

void Skybox::set_texture(const string& texture) { mTexture = texture; }

void Skybox::set_saturation(float saturation) { mSaturation = saturation; }

void Skybox::set_brightness(float brightness) { mBrightness = brightness; }

void Skybox::set_contrast(float contrast) { mContrast = contrast; }

void Skybox::set_opacity(float opacity) { mOpacity = opacity; }
