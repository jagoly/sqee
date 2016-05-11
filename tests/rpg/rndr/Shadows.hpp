#pragma once

#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Shaders.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>

#include "Renderer.hpp"

namespace sqt { namespace rndr {

class Shadows final : sq::NonCopyable {
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
    void draw_ModelSimple(const Mat4F& _lightMat, const ModelComponent* _modelC);
    void draw_ModelSimple_punch(const Mat4F& _lightMat, const ModelComponent* _modelC);
    void draw_ModelSkelly(const Mat4F& _lightMat, const ModelComponent* _modelC);
    void draw_ModelSkelly_punch(const Mat4F& _lightMat, const ModelComponent* _modelC);
};

}}
