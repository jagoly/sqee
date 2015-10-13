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

    sq::Texture2D TEX_ssao {gl::RED, gl::R8, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_ssaoB {gl::RED, gl::R8, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_bloom {gl::RGB, gl::RGB8, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_bloomB {gl::RGB, gl::RGB8, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_shafts {gl::RED, gl::R8, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_final {gl::RGBA, gl::RGBA8, sq::Texture::LinearClamp()};

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
