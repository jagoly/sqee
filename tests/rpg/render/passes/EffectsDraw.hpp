#pragma once

#include <sqee/gl/FrameBuffer.hpp>

#include "../SharedStuff.hpp"

namespace sqt { namespace render {

//============================================================================//

class EffectsPasses : public SharedStuff
{
public:

    //========================================================//

    EffectsPasses(const SharedStuff& stuff);

    void update_options();

    void setup_render_state();

    void render_effect_SSAO();
    void render_effect_Bloom();
    void render_effect_Shafts();

    void render_effect_Overlay();

private:

    //========================================================//

    sq::FrameBuffer FB_SSAO_Main, FB_SSAO_Blur;
    sq::FrameBuffer FB_Bloom_Main, FB_Bloom_Blur;
    sq::FrameBuffer FB_Shafts_Main, FB_Shafts_Blur;

    sq::Texture2D TEX_SSAO_Blur { sq::Texture::Format::R8_UN };
    sq::Texture2D TEX_Bloom_Blur { sq::Texture::Format::RGB8_UN };
    sq::Texture2D TEX_Shafts_Blur { sq::Texture::Format::RGB8_UN };

    sq::Shader FS_SSAO_Main { sq::Shader::Stage::Fragment };
    sq::Shader FS_SSAO_Blur { sq::Shader::Stage::Fragment };

    sq::Shader FS_Bloom_Main { sq::Shader::Stage::Fragment };
    sq::Shader FS_Bloom_BlurH { sq::Shader::Stage::Fragment };
    sq::Shader FS_Bloom_BlurV { sq::Shader::Stage::Fragment };

    sq::Shader FS_Shafts_BlurH { sq::Shader::Stage::Fragment };
    sq::Shader FS_Shafts_BlurV { sq::Shader::Stage::Fragment };

    sq::Shader FS_Overlay { sq::Shader::Stage::Fragment };
};

//============================================================================//

}} // namespace sqt::render
