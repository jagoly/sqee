#pragma once

#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Shaders.hpp>
#include <sqee/maths/Vectors.hpp>

#include "Renderer.hpp"

namespace sqt { namespace rndr {

class Reflects final : sq::NonCopyable {
public:
    Reflects(const Renderer& _renderer);

    void update_settings();

    void render_reflections();

private:
    const Renderer& renderer;

    sq::FrameBuffer FB_stencil;
    sq::FrameBuffer FB_reflGbuf;
    sq::FrameBuffer FB_reflHdr;

    sq::Texture2D TEX_reflDiff {gl::RGB, gl::RGB8, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_reflSurf {gl::RGB, gl::RGB8, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_reflDpSt {gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, sq::Texture::LinearClamp()};
    sq::Texture2D TEX_reflHdr {gl::RGB, gl::RGB16F, sq::Texture::LinearClamp()};

    sq::Shader VS_defr_reflector {gl::VERTEX_SHADER};
    sq::Shader FS_defr_reflector {gl::FRAGMENT_SHADER};

    Vec2U INFO_fullSize, INFO_halfSize;
};

}}
