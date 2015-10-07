#pragma once
#include <sqee/forward.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Shaders.hpp>

#include "Renderer.hpp"

namespace sqt {
namespace rndr {

class Pretties final : NonCopyable {
friend class Lighting;
public:
    Pretties(const Renderer& _renderer);

    void update_settings();

    void setup_render_state();

    void render_ssao_texture();
    void render_bloom_texture();
    void render_shafts_texture();

    void render_hdr_tonemap();
    void render_fsaa_screen();
    void render_vignette();

private:
    const Renderer& renderer;

    sq::FrameBuffer FB_simple;

    // todo: make non pointers
    unique_ptr<sq::Texture2D> TEX_ssao;
    unique_ptr<sq::Texture2D> TEX_ssaoB;
    unique_ptr<sq::Texture2D> TEX_bloom;
    unique_ptr<sq::Texture2D> TEX_bloomB;
    unique_ptr<sq::Texture2D> TEX_shafts;
    unique_ptr<sq::Texture2D> TEX_final;

    sq::Shader FS_prty_ssao_ssao {gl::FRAGMENT_SHADER};
    sq::Shader FS_prty_ssao_blur {gl::FRAGMENT_SHADER};

    sq::Shader VS_prty_shafts_shafts {gl::VERTEX_SHADER};
    sq::Shader FS_prty_shafts_shafts {gl::FRAGMENT_SHADER};
    sq::Shader FS_prty_shafts_write {gl::FRAGMENT_SHADER};

    sq::Shader FS_prty_hdr_highs {gl::FRAGMENT_SHADER};
    sq::Shader FS_prty_hdr_blurh {gl::FRAGMENT_SHADER};
    sq::Shader FS_prty_hdr_blurv {gl::FRAGMENT_SHADER};
    sq::Shader FS_prty_hdr_tones {gl::FRAGMENT_SHADER};

    sq::Shader FS_prty_fsaa_last {gl::FRAGMENT_SHADER};
    sq::Shader FS_prty_vignette {gl::FRAGMENT_SHADER};
};

}}
