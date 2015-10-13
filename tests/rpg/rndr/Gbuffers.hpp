#pragma once
#include <sqee/forward.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Shaders.hpp>

#include "Renderer.hpp"

namespace sqt {
namespace rndr {

class Gbuffers final : NonCopyable {
friend class Lighting;
friend class Reflects;
friend class Pretties;
public:
    Gbuffers(const Renderer& _renderer);

    void update_settings();

    void setup_render_state_base();
    void setup_render_state_refl();

    void bind_textures_base();
    void bind_textures_refl();

    void render_reflects_base(bool _decals);
    void render_msimples_base(bool _decals);
    void render_mskellys_base(bool _decals);
    void render_decals_base();

    void render_reflects_refl(const ReflectorData& _data, bool _decals);
    void render_msimples_refl(const ReflectorData& _data, bool _decals);
    void render_mskellys_refl(const ReflectorData& _data, bool _decals);
    void render_decals_refl(const ReflectorData& _data);

    void render_downscaled_depth();

private:
    const Renderer& renderer;

    sq::FrameBuffer FB_baseGbuf;
    sq::FrameBuffer FB_reflGbuf;
    sq::FrameBuffer FB_depth;

    sq::Texture2D TEX_baseDiff {gl::RGB, gl::RGB8, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_baseSurf {gl::RGB, gl::RGB10, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_baseNorm {gl::RGB, gl::RGB10, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_baseSpec {gl::RGB, gl::RGB8, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_baseDpSt {gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_reflDiff {gl::RGB, gl::RGB8, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_reflSurf {gl::RGB, gl::RGB8, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_reflDpSt {gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_depHalf {gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT24, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_depQter {gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT24, sq::Texture::LinearClamp()};

    sq::Shader VS_gbuf_base_simple {gl::VERTEX_SHADER};
    sq::Shader VS_gbuf_base_skelly {gl::VERTEX_SHADER};
    sq::Shader VS_gbuf_base_decal {gl::VERTEX_SHADER};
    sq::Shader FS_gbuf_base_model {gl::FRAGMENT_SHADER};
    sq::Shader FS_gbuf_base_decal {gl::FRAGMENT_SHADER};

    sq::Shader VS_gbuf_refl_simple {gl::VERTEX_SHADER};
    sq::Shader VS_gbuf_refl_skelly {gl::VERTEX_SHADER};
    sq::Shader VS_gbuf_refl_decal {gl::VERTEX_SHADER};
    sq::Shader FS_gbuf_refl_model {gl::FRAGMENT_SHADER};
    sq::Shader FS_gbuf_refl_decal {gl::FRAGMENT_SHADER};
};

}}
