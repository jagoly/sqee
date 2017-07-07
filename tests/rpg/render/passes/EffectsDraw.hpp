#pragma once

#include "../SharedStuff.hpp"

namespace sqt::render {

//============================================================================//

class EffectsPasses : public SharedStuff
{
public: //====================================================//

    EffectsPasses(const SharedStuff& stuff);

    void update_options();

    void setup_render_state();

    void render_effect_SSAO();
    void render_effect_Bloom();
    void render_effect_Shafts();

    void render_effect_Overlay();

private: //===================================================//

    sq::FrameBuffer FB_SSAO_Main, FB_SSAO_Blur;
    sq::FrameBuffer FB_Bloom_Main, FB_Bloom_Blur;
    sq::FrameBuffer FB_Shafts_Main, FB_Shafts_Blur;

    sq::Texture2D TEX_SSAO_Blur { sq::Texture::Format::R8_UN };
    sq::Texture2D TEX_Bloom_Blur { sq::Texture::Format::RGB8_UN };
    sq::Texture2D TEX_Shafts_Blur { sq::Texture::Format::RGB8_UN };

    sq::Program PROG_SSAO_Main;
    sq::Program PROG_SSAO_Blur;
    sq::Program PROG_Bloom_Main;
    sq::Program PROG_Bloom_BlurH;
    sq::Program PROG_Bloom_BlurV;
    sq::Program PROG_Shafts_BlurH;
    sq::Program PROG_Shafts_BlurV;
    sq::Program PROG_Overlay;
};

//============================================================================//

} // namespace sqt::render
