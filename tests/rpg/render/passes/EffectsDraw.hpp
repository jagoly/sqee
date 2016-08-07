#pragma once

#include <sqee/gl/FrameBuffer.hpp>

#include "../SharedStuff.hpp"

namespace sqt { namespace render {

class EffectsPasses final : public SharedStuff {
public:
    EffectsPasses(const SharedStuff& _stuff);

    void update_options();

    void setup_render_state();

    void render_effect_SSAO();
    void render_effect_Bloom();
    void render_effect_Shafts();

    void render_effect_Overlay();

private:

    sq::FrameBuffer FB_SSAO_Main, FB_SSAO_Blur;
    sq::FrameBuffer FB_Bloom_Main, FB_Bloom_Blur;
    sq::FrameBuffer FB_Shafts_Main, FB_Shafts_Blur;

    sq::Texture2D TEX_SSAO_Blur {gl::RED, gl::R8, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_Bloom_Blur {gl::RGB, gl::RGB8, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_Shafts_Blur {gl::RGB, gl::RGB8, sq::Texture::LinearClamp()};

    sq::Shader FS_SSAO_Main {gl::FRAGMENT_SHADER};
    sq::Shader FS_SSAO_Blur {gl::FRAGMENT_SHADER};

    sq::Shader FS_Bloom_Main {gl::FRAGMENT_SHADER};
    sq::Shader FS_Bloom_BlurH {gl::FRAGMENT_SHADER};
    sq::Shader FS_Bloom_BlurV {gl::FRAGMENT_SHADER};

    sq::Shader FS_Shafts_BlurH {gl::FRAGMENT_SHADER};
    sq::Shader FS_Shafts_BlurV {gl::FRAGMENT_SHADER};

    sq::Shader FS_Overlay {gl::FRAGMENT_SHADER};
};

}}
