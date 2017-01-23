#include <sqee/gl/Context.hpp>

using namespace sq;

//============================================================================//

Context::Context()
{
    gl::CreateVertexArrays(1, &mDummyVAO);
    gl::CreateProgramPipelines(1, &mPipeline);
}

Context::~Context()
{
    gl::DeleteVertexArrays(1, &mDummyVAO);
    gl::DeleteProgramPipelines(1, &mPipeline);
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

Context& Context::get()
{
    static Context instance;
    return instance;
}
