#pragma once
#include <sqee/forward.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Shaders.hpp>

#include "Renderer.hpp"

namespace sqt {
namespace rndr {

class Shadows final : NonCopyable {
public:
    Shadows(const Renderer& _renderer);

    void update_settings();

    void setup_render_state();

    void render_shadows_sky();
    void render_shadows_spot();
    void render_shadows_point();

private:
    const Renderer& renderer;

    sq::FrameBuffer FB_shadows;

    sq::Shader VS_shad_simple {gl::VERTEX_SHADER};
    sq::Shader VS_shad_skelly {gl::VERTEX_SHADER};
    sq::Shader FS_shad_punch {gl::FRAGMENT_SHADER};

    // Helpers /////
    void draw_Reflector(fmat4 _lightMat, const wcoe::Reflector& _rflct);
    void draw_ModelSimple(fmat4 _lightMat, const wcoe::ModelSimple& _model);
    void draw_ModelSkelly(fmat4 _lightMat, const wcoe::ModelSkelly& _model);
    void draw_Reflector_punch(fmat4 _lightMat, const wcoe::Reflector& _rflct);
    void draw_ModelSimple_punch(fmat4 _lightMat, const wcoe::ModelSimple& _model);
    void draw_ModelSkelly_punch(fmat4 _lightMat, const wcoe::ModelSkelly& _model);
};

}}
