#include <sqee/gl/Context.hpp>
#include <sqee/gl/Drawing.hpp>

using namespace sq;

//============================================================================//

void sq::draw_nothing()
{
    static auto& context = Context::get();

    context.bind_VertexArray_dummy();

    gl::DrawArrays(gl::POINTS, 0, 1);
}

//============================================================================//

void sq::draw_screen_quad()
{
    static auto& context = Context::get();

    context.bind_VertexArray_dummy();

    gl::DrawArrays(gl::TRIANGLE_STRIP, 0, 4);
}
