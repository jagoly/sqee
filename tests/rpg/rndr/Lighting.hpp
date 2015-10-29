#pragma once

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Shaders.hpp>
#include <sqee/maths/Vectors.hpp>

#include "Renderer.hpp"

namespace sqt { namespace rndr {

class Lighting final : sq::NonCopyable {
public:
    Lighting(const Renderer& _renderer);

    void update_settings();

    void render_lighting_base();
    void render_lighting_refl(const ReflectorData& _data);

private:
    const Renderer& renderer;

    sq::FrameBuffer FB_baseHdr;

    sq::Texture2D TEX_baseHdr {gl::RGB, gl::RGB16F, sq::Texture::LinearClamp()};

    sq::Shader VS_defr_base_skybox {gl::VERTEX_SHADER};
    sq::Shader FS_defr_base_ambient {gl::FRAGMENT_SHADER};
    sq::Shader FS_defr_base_skylight {gl::FRAGMENT_SHADER};
    sq::Shader FS_defr_base_spot_none {gl::FRAGMENT_SHADER};
    sq::Shader FS_defr_base_spot_shad {gl::FRAGMENT_SHADER};
    sq::Shader FS_defr_base_spot_spec {gl::FRAGMENT_SHADER};
    sq::Shader FS_defr_base_spot_both {gl::FRAGMENT_SHADER};
    sq::Shader FS_defr_base_point_none {gl::FRAGMENT_SHADER};
    sq::Shader FS_defr_base_point_shad {gl::FRAGMENT_SHADER};
    sq::Shader FS_defr_base_point_spec {gl::FRAGMENT_SHADER};
    sq::Shader FS_defr_base_point_both {gl::FRAGMENT_SHADER};

    sq::Shader VS_defr_refl_skybox {gl::VERTEX_SHADER};
    sq::Shader FS_defr_refl_ambient {gl::FRAGMENT_SHADER};
    sq::Shader FS_defr_refl_skylight {gl::FRAGMENT_SHADER};
    sq::Shader FS_defr_refl_spot_none {gl::FRAGMENT_SHADER};
    sq::Shader FS_defr_refl_spot_shad {gl::FRAGMENT_SHADER};
    sq::Shader FS_defr_refl_point_none {gl::FRAGMENT_SHADER};
    sq::Shader FS_defr_refl_point_shad {gl::FRAGMENT_SHADER};

    sq::Shader FS_defr_skybox {gl::FRAGMENT_SHADER};

    Vec2U INFO_fullSize, INFO_halfSize;
    int INFO_ssao;

    friend class Pretties;
    friend class Reflects;
};

}}
