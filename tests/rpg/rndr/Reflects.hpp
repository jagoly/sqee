#pragma once
#include <sqee/forward.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Shaders.hpp>

#include "Renderer.hpp"

namespace sqt {
namespace rndr {

class Reflects final : NonCopyable {
public:
    Reflects(const Renderer& _renderer);

    void update_settings();

    void render_reflections();

private:
    const Renderer& renderer;

    sq::FrameBuffer FB_stencil;

    sq::Shader VS_defr_reflector {gl::VERTEX_SHADER};
    sq::Shader FS_defr_reflector {gl::FRAGMENT_SHADER};
};

}}
