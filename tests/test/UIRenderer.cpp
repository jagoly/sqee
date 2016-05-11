/*#include <JagolyGUI/Widget.hpp>
#include <JagolyGUI/Container.hpp>
#include <JagolyGUI/RenderTree.hpp>
#include <JagolyGUI/containers/Frame.hpp>
#include <JagolyGUI/containers/BoxH.hpp>
#include <JagolyGUI/containers/BoxV.hpp>
#include <JagolyGUI/widgets/Button.hpp>
#include <JagolyGUI/widgets/Label.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/gl/Shaders.hpp>

#include "UIRenderer.hpp"

using namespace sqt;
namespace gui = jagolygui;

extern const char ui_VS_box_simple[];
extern const char ui_FS_colour_simple[];


template<class T> static inline
Vec2F scale_vec2(gui::Vec2<T> _crd) {
    return Vec2F(_crd.x, _crd.y) / Vec2F(800.f, 600.f);
}


struct UIRenderer::Implementation {
    Implementation();

    gui::RenderTree renderTree;

    void prepare_to_render();

    void render_Widget(gui::Widget* _widget);
    void render_Label(gui::Widget* _widget);

    sq::Shader VS_box_simple {gl::VERTEX_SHADER};
    sq::Shader FS_colour_simple {gl::FRAGMENT_SHADER};

    sq::Pipeline pipeline;
    GLuint dummyVAO = 0u;
};

void UIRenderer::Implementation::prepare_to_render() {
    pipeline.bind();
}


UIRenderer::Implementation::Implementation() {
    VS_box_simple.add_uniform("posSize"); // vec4
    FS_colour_simple.add_uniform("colour"); // vec3

    VS_box_simple.load(ui_VS_box_simple);
    FS_colour_simple.load(ui_FS_colour_simple);

    using This = UIRenderer::Implementation;

    renderTree.register_widget<gui::Widget>(&This::render_Widget, this);
    renderTree.register_widget<gui::Label>(&This::render_Label, this);
    renderTree.register_widget<gui::Button, gui::Widget>();
    renderTree.register_widget<gui::Frame, gui::Widget>();
    renderTree.register_widget<gui::BoxV, gui::Widget>();
    renderTree.register_widget<gui::BoxH, gui::Widget>();

    gl::GenVertexArrays(1, &dummyVAO);
}


void UIRenderer::Implementation::render_Widget(gui::Widget* _w) {
    pipeline.use_shader(VS_box_simple);
    pipeline.use_shader(FS_colour_simple);

    gl::BindVertexArray(dummyVAO);

    Vec2F pos = scale_vec2(_w->calculatedPos);
    Vec2F size = scale_vec2(_w->calculatedSize);

    Vec4F posSize = Vec4F(pos, size);
    Vec3F colour = {1.f, 0.f, 0.f};

    if (_w->has_hover()) colour = {0.f, 0.f, 1.f};

    VS_box_simple.set_vec("posSize", posSize);
    FS_colour_simple.set_vec("colour", colour);
    gl::DrawArrays(gl::TRIANGLE_FAN, 0, 4);

    colour = {1.f, 1.f, 1.f};
    FS_colour_simple.set_vec("colour", colour);
    gl::DrawArrays(gl::LINE_LOOP, 0, 4);
}


void UIRenderer::Implementation::render_Label(gui::Widget* _w) {
    pipeline.use_shader(VS_box_simple);
    pipeline.use_shader(FS_colour_simple);

    gl::BindVertexArray(dummyVAO);

    Vec2F pos = scale_vec2(_w->calculatedPos);
    Vec2F size = scale_vec2(_w->calculatedSize);

    Vec4F posSize = Vec4F(pos, size);
    Vec3F colour = {1.f, 1.f, 0.f};

    VS_box_simple.set_vec("posSize", posSize);
    FS_colour_simple.set_vec("colour", colour);
    gl::DrawArrays(gl::TRIANGLE_FAN, 0, 4);

    colour = {1.f, 1.f, 1.f};
    FS_colour_simple.set_vec("colour", colour);
    gl::DrawArrays(gl::LINE_LOOP, 0, 4);
}


UIRenderer::UIRenderer() : impl(new Implementation()) {}
UIRenderer::~UIRenderer() = default;

void UIRenderer::render_gui(gui::Widget* _root) {
    impl->prepare_to_render();
    impl->renderTree.render_widget(_root);
}
*/
