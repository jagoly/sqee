#include <sqee/gl/Context.hpp>

using namespace sq;

//============================================================================//

Context::Context()
{
    gl::CreateVertexArrays(1, &mDummyVAO);
}

Context::~Context()
{
    gl::DeleteVertexArrays(1, &mDummyVAO);
}

//============================================================================//

void Context::clear_Colour(Vec4F colour)
{
    gl::ClearColor(colour.r, colour.g, colour.b, colour.a);
    gl::Clear(gl::COLOR_BUFFER_BIT);
}

void Context::clear_Depth(double depth)
{
    if (crnt_Depth_Test != Depth_Test::Replace) gl::DepthMask(true);

    gl::ClearDepth(depth);
    gl::Clear(gl::DEPTH_BUFFER_BIT);

    if (crnt_Depth_Test == Depth_Test::Keep) gl::DepthMask(false);
}

void Context::clear_Stencil(uchar stencil, uchar mask)
{
    if (crnt_Stencil_Params.w != mask) gl::StencilMask(mask);

    gl::ClearStencil(stencil);
    gl::Clear(gl::STENCIL_BUFFER_BIT);

    if (crnt_Stencil_Params.w != mask) gl::StencilMask(crnt_Stencil_Params.w);
}

void Context::clear_Depth_Stencil()
{
    if (crnt_Depth_Test != Depth_Test::Replace) gl::DepthMask(true);
    if (crnt_Stencil_Params.w != 255u) gl::StencilMask(255u);

    gl::ClearDepth(1.0); gl::ClearStencil(0u);
    gl::Clear(gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT);

    if (crnt_Depth_Test == Depth_Test::Keep) gl::DepthMask(false);
    if (crnt_Stencil_Params.w != 255u) gl::StencilMask(crnt_Stencil_Params.w);
}

//============================================================================//

void Context::impl_reset_FrameBuffer(const FrameBuffer* old, const FrameBuffer* fresh)
{
    if (mFrameBufferBindingRead == old) mFrameBufferBindingRead = fresh;
    if (mFrameBufferBindingDraw == old) mFrameBufferBindingDraw = fresh;
}

void Context::impl_reset_Program(const Program* old, const Program* fresh)
{
    if (mProgramBinding == old) mProgramBinding = fresh;
}

void Context::impl_reset_Texture(const Texture* old, const Texture* fresh)
{
    for (auto& reference : mTextureBindings)
        if (reference == old) reference = fresh;
}

void Context::impl_reset_UniformBuffer(const UniformBuffer* old, const UniformBuffer* fresh)
{
    for (auto& binding : mUniformBufferBindings)
        if (binding.ptr == old) binding.ptr = fresh;
}

void Context::impl_reset_VertexArray(const VertexArray* old, const VertexArray* fresh)
{
    if (mVertexArrayBinding == old) mVertexArrayBinding = fresh;
}

//============================================================================//

Context& Context::get()
{
    thread_local Context instance;
    return instance;
}
