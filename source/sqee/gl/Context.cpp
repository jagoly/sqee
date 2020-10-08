#include <sqee/gl/Context.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>

#include <sqee/gl/FixedBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Functions.hpp>
#include <sqee/gl/Program.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/VertexArray.hpp>

using namespace sq;

//============================================================================//

Context::Context()
{
    sq::log_info("Creating SQEE Render Context");
    gl::CreateVertexArrays(1, &mDummyVAO);
}

Context::~Context()
{
    sq::log_info("Destroying SQEE Render Context");
    gl::DeleteVertexArrays(1, &mDummyVAO);
}

Context& Context::get()
{
    thread_local Context instance;
    return instance;
}

//============================================================================//

void Context::set_state(AlphaCoverage state)
{
    if (bool(state)) gl::Enable(gl::SAMPLE_ALPHA_TO_COVERAGE);
    else gl::Disable(gl::SAMPLE_ALPHA_TO_COVERAGE);
}

void Context::set_state(ClipDistance state)
{
    if (bool(state)) gl::Enable(gl::CLIP_DISTANCE0);
    else gl::Disable(gl::CLIP_DISTANCE0);
}

void Context::set_state(DepthClamp state)
{
    if (bool(state)) gl::Enable(gl::DEPTH_CLAMP);
    else gl::Disable(gl::DEPTH_CLAMP);
}

void Context::set_state(ScissorTest state)
{
    if (bool(state)) gl::Enable(gl::SCISSOR_TEST);
    else gl::Disable(gl::SCISSOR_TEST);
}

//============================================================================//

void Context::set_state(BlendMode state)
{
    if (state != BlendMode::Disable)
    {
        gl::Enable(gl::BLEND);
        if (state == BlendMode::Accumulate) gl::BlendFunc(gl::ONE, gl::ONE);
        if (state == BlendMode::Alpha) gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
        if (state == BlendMode::PremAlpha) gl::BlendFunc(gl::ONE, gl::ONE_MINUS_SRC_ALPHA);
        if (state == BlendMode::Custom) {} // set_blend_custom
    }
    else gl::Disable(gl::BLEND);
}

void Context::set_state(CullFace state)
{
    if (state != CullFace::Disable)
    {
        gl::Enable(gl::CULL_FACE);
        if (state == CullFace::Back) gl::CullFace(gl::BACK);
        if (state == CullFace::Front) gl::CullFace(gl::FRONT);
    }
    else gl::Disable(gl::CULL_FACE);
}

void Context::set_state(DepthTest state)
{
    if (state != DepthTest::Disable)
    {
        gl::Enable(gl::DEPTH_TEST);
        if (state == DepthTest::Keep) gl::DepthMask(false);
        if (state == DepthTest::Replace) gl::DepthMask(true);
    }
    else gl::Disable(gl::DEPTH_TEST);
}

void Context::set_state(StencilTest state)
{
    if (state != StencilTest::Disable)
    {
        gl::Enable(gl::STENCIL_TEST);
        if (state == StencilTest::Keep) gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
        if (state == StencilTest::Replace) gl::StencilOp(gl::KEEP, gl::KEEP, gl::REPLACE);
        if (state == StencilTest::Custom) {} // set_stencil_custom
    }
    else gl::Disable(gl::STENCIL_TEST);
}

//============================================================================//

void Context::set_scissor_box(uint x, uint y, uint width, uint height)
{
    gl::Scissor(int(x), int(y), int(width), int(height));
}

void Context::set_depth_compare(CompareFunc func)
{
    gl::DepthFunc(GLenum(func));
}

void Context::set_stencil_params(CompareFunc func, uchar ref, uchar readMask, uchar writeMask)
{
    gl::StencilFunc(GLenum(func), int(ref), uint(readMask));
    gl::StencilMask(uint(writeMask));
}

void Context::set_blend_custom(BlendFactor src, BlendFactor dest)
{
    gl::BlendFunc(GLenum(src), GLenum(dest));
}

void Context::set_stencil_custom(StencilOp fail, StencilOp zFail, StencilOp zPass)
{
    gl::StencilOp(GLenum(fail), GLenum(zFail), GLenum(zPass));
}

//============================================================================//

void Context::set_line_width(float width)
{
    gl::LineWidth(width);
}

//============================================================================//

void Context::set_ViewPort(Vec2U size)
{
    mCurrentViewPort = size;
    gl::Viewport(0, 0, int(size.x), int(size.y));
}

//============================================================================//

void Context::bind_buffer(const FixedBuffer& buffer, BufTarget target, uint slot)
{
    SQASSERT(buffer.get_handle() != 0u, "");
    gl::BindBufferBase(GLenum(target), slot, buffer.get_handle());
}

void Context::bind_buffer(const FixedBuffer& buffer, BufTarget target, uint slot, uint offset, uint size)
{
    SQASSERT(buffer.get_handle() != 0u, "");
    gl::BindBufferRange(GLenum(target), slot, buffer.get_handle(), ptrdiff_t(offset), ptrdiff_t(size));
}

void Context::bind_framebuffer(const FrameBuffer& fbo, FboTarget target)
{
    SQASSERT(fbo.get_handle() != 0u, "");
    gl::BindFramebuffer(GLenum(target), fbo.get_handle());
}

void Context::bind_program(const Program& program)
{
    SQASSERT(program.get_handle() != 0u, "");
    gl::UseProgram(program.get_handle());
}

void Context::bind_texture(const Texture& tex, uint slot)
{
    SQASSERT(tex.get_handle() != 0u, "");
    gl::BindTextureUnit(slot, tex.get_handle());
}

void Context::bind_vertexarray(const VertexArray& vao)
{
    SQASSERT(vao.get_handle() != 0u, "");
    gl::BindVertexArray(vao.get_handle());
}

//============================================================================//

void Context::bind_framebuffer_default(FboTarget target)
{
    gl::BindFramebuffer(GLenum(target), 0u);
}

void Context::bind_program_default()
{
    gl::UseProgram(0u);
}

void Context::bind_vertexarray_dummy()
{
    gl::BindVertexArray(mDummyVAO);
}

//============================================================================//

void Context::clear_depth(double depth)
{
    gl::DepthMask(true);
    gl::ClearDepth(depth);
    gl::Clear(gl::DEPTH_BUFFER_BIT);
}

void Context::clear_stencil(uchar stencil, uchar mask)
{
    gl::StencilMask(uint(mask));
    gl::ClearStencil(int(stencil));
    gl::Clear(gl::STENCIL_BUFFER_BIT);
}

void Context::clear_colour(Vec4F colour)
{
    gl::ColorMask(true, true, true, true);
    gl::ClearColor(colour.r, colour.g, colour.b, colour.a);
    gl::Clear(gl::COLOR_BUFFER_BIT);
}

void Context::clear_depth_stencil(double depth, uchar stencil, uchar mask)
{
    gl::DepthMask(true);
    gl::ClearDepth(depth);
    gl::StencilMask(uint(mask));
    gl::ClearStencil(int(stencil));
    gl::Clear(gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT);
}

void Context::clear_depth_colour(double depth, Vec4F colour)
{
    gl::DepthMask(true);
    gl::ClearDepth(depth);
    gl::ColorMask(true, true, true, true);
    gl::ClearColor(colour.r, colour.g, colour.b, colour.a);
    gl::Clear(gl::DEPTH_BUFFER_BIT | gl::COLOR_BUFFER_BIT);
}

void Context::clear_stencil_colour(uchar stencil, uchar mask, Vec4F colour)
{
    gl::StencilMask(uint(mask));
    gl::ClearStencil(int(stencil));
    gl::ColorMask(true, true, true, true);
    gl::ClearColor(colour.r, colour.g, colour.b, colour.a);
    gl::Clear(gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT | gl::COLOR_BUFFER_BIT);
}

void Context::clear_depth_stencil_colour(double depth, uchar stencil, uchar mask, Vec4F colour)
{
    gl::DepthMask(true);
    gl::ClearDepth(depth);
    gl::StencilMask(uint(mask));
    gl::ClearStencil(int(stencil));
    gl::ColorMask(true, true, true, true);
    gl::ClearColor(colour.r, colour.g, colour.b, colour.a);
    gl::Clear(gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT | gl::COLOR_BUFFER_BIT);
}

//============================================================================//

void Context::draw_arrays(DrawPrimitive primitive, uint first, uint count)
{
    gl::DrawArrays(GLenum(primitive), int(first), int(count));
}

void Context::draw_elements_u8(DrawPrimitive primitive, uint8_t first, uint8_t count)
{
    const ptrdiff_t indices = ptrdiff_t(first * 1u);
    gl::DrawElements(GLenum(primitive), int(count), gl::UNSIGNED_BYTE, reinterpret_cast<const void*>(indices));
}

void Context::draw_elements_u16(DrawPrimitive primitive, uint16_t first, uint16_t count)
{
    const ptrdiff_t indices = ptrdiff_t(first * 2u);
    gl::DrawElements(GLenum(primitive), int(count), gl::UNSIGNED_SHORT, reinterpret_cast<const void*>(indices));
}

void Context::draw_elements_u32(DrawPrimitive primitive, uint32_t first, uint32_t count)
{
    const ptrdiff_t indices = ptrdiff_t(first * 4u);
    gl::DrawElements(GLenum(primitive), int(count), gl::UNSIGNED_INT, reinterpret_cast<const void*>(indices));
}
