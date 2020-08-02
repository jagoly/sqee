#include <sqee/gl/Drawing.hpp>

#include <sqee/gl/Context.hpp>
#include <sqee/redist/gl_loader.hpp>

using namespace sq;

//============================================================================//

void sq::draw_nothing()
{
    Context::get().bind_VertexArray_dummy();

    gl::DrawArrays(gl::POINTS, 0, 1);
}

//============================================================================//

void sq::draw_screen_quad()
{
    Context::get().bind_VertexArray_dummy();

    gl::DrawArrays(gl::TRIANGLE_STRIP, 0, 4);
}
