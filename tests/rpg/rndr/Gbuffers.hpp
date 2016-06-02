#pragma once

#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Shaders.hpp>

#include "Renderer.hpp"

namespace sqt {

class Renderer::Gbuffers : sq::NonCopyable {
public:
    Gbuffers(const Renderer& _renderer);

    void update_options();

    void render_gbuffers_base();

    void render_gbuffers_refl(const ReflectData& _data);

private:
    const Renderer& renderer;
    const RpgOptions& options;

    sq::FrameBuffer FB_baseGbuf;
    sq::FrameBuffer FB_depth;

    sq::Texture2D TEX_baseDiff {gl::RGB, gl::RGB8, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_baseSurf {gl::RGB, gl::RGB10, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_baseNorm {gl::RGB, gl::RGB10, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_baseSpec {gl::RGB, gl::RGB8, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_baseDpSt {gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, sq::Texture::LinearClamp()};
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

    Vec2U INFO_fullSize, INFO_halfSize, INFO_qterSize;

    void draw_msimples_base(bool _decals);
    void draw_mskellys_base(bool _decals);

    void draw_msimples_refl(const ReflectData& _data, bool _decals);
    void draw_mskellys_refl(const ReflectData& _data, bool _decals);

    friend class Renderer::Lighting;
    friend class Renderer::Reflects;
    friend class Renderer::Pretties;
};

}
