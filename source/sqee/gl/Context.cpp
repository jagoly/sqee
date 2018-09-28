// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#include <sqee/gl/Context.hpp>

#include <sqee/debug/Logging.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/PipelineState.hpp>
#include <sqee/gl/Program.hpp>
#include <sqee/gl/TextureBase.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/VertexArray.hpp>

#include <sqee/redist/gl_loader.hpp>

using namespace sq;

//============================================================================//

Context::Context()
{
    crnt_Scissor_Params = { 0u, 0u, 0u, 0u };
    crnt_Stencil_Params = { gl::ALWAYS, 0u, 255u, 255u };
    crnt_Stencil_Custom = { gl::KEEP, gl::KEEP, gl::KEEP };
    crnt_Blend_Custom   = { gl::ONE, gl::ZERO };
    crnt_ViewPort       = { 0u, 0u };

    sq::log_info("Creating SQEE Render Context");
    gl::CreateVertexArrays(1, &mDummyVAO);
}

Context::~Context()
{
    sq::log_info("Destroying SQEE Render Context");
    gl::DeleteVertexArrays(1, &mDummyVAO);
}

//============================================================================//

void Context::set_state(Clip_Distance state)
{
    if (impl_check_modify(crnt_Clip_Distance, state)) return;

    if (state == Clip_Distance::Disable) gl::Disable(gl::CLIP_DISTANCE0);
    if (state == Clip_Distance::Enable)  gl::Enable(gl::CLIP_DISTANCE0);
}

void Context::set_state(Depth_Clamp state)
{
    if (impl_check_modify(crnt_Depth_Clamp, state)) return;

    if (state == Depth_Clamp::Disable) gl::Disable(gl::DEPTH_CLAMP);
    if (state == Depth_Clamp::Enable)  gl::Enable(gl::DEPTH_CLAMP);
}

void Context::set_state(Scissor_Test state)
{
    if (impl_check_modify(crnt_Scissor_Test, state)) return;

    if (state == Scissor_Test::Disable) gl::Disable(gl::SCISSOR_TEST);
    if (state == Scissor_Test::Enable)  gl::Enable(gl::SCISSOR_TEST);
}

//void Context::set_state(Alpha_Coverage state)
//{
//    if (impl_check_modify(crnt_Alpha_Coverage, state)) return;
//
//    if (state == Alpha_Coverage::Disable) gl::Disable(gl::SAMPLE_ALPHA_TO_COVERAGE);
//    if (state == Alpha_Coverage::Enable)  gl::Enable(gl::SAMPLE_ALPHA_TO_COVERAGE);
//}

void Context::set_state(Cull_Face state)
{
    if (impl_check_modify(crnt_Cull_Face, state)) return;

    if (state == Cull_Face::Disable) { gl::Disable(gl::CULL_FACE); return; }

    gl::Enable(gl::CULL_FACE);

    if (state == Cull_Face::Back)  gl::CullFace(gl::BACK);
    if (state == Cull_Face::Front) gl::CullFace(gl::FRONT);
}

void Context::set_state(Depth_Test state)
{
    if (impl_check_modify(crnt_Depth_Test, state)) return;

    if (state == Depth_Test::Disable) { gl::Disable(gl::DEPTH_TEST); return; }

    gl::Enable(gl::DEPTH_TEST);

    if (state == Depth_Test::Keep)    gl::DepthMask(false);
    if (state == Depth_Test::Replace) gl::DepthMask(true);
}

void Context::set_state(Stencil_Test state)
{
    if (impl_check_modify(crnt_Stencil_Test, state)) return;

    if (state == Stencil_Test::Disable) { gl::Disable(gl::STENCIL_TEST); return; }

    gl::Enable(gl::STENCIL_TEST);

    if (state == Stencil_Test::Keep)    set_Stencil_Custom({gl::KEEP, gl::KEEP, gl::KEEP});
    if (state == Stencil_Test::Replace) set_Stencil_Custom({gl::KEEP, gl::KEEP, gl::REPLACE});
}

void Context::set_state(Blend_Mode state)
{
    if (impl_check_modify(crnt_Blend_Mode, state)) return;

    if (state == Blend_Mode::Disable) { gl::Disable(gl::BLEND); return; }

    gl::Enable(gl::BLEND);

    if (state == Blend_Mode::Accumulate) set_Blend_Custom({gl::ONE, gl::ONE});
    if (state == Blend_Mode::Alpha)      set_Blend_Custom({gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA});
    if (state == Blend_Mode::PremAlpha)  set_Blend_Custom({gl::ONE, gl::ONE_MINUS_SRC_ALPHA});
}

void Context::set_state(Depth_Compare state)
{
    if (impl_check_modify(crnt_Depth_Compare, state)) return;

    if (state == Depth_Compare::Less)         gl::DepthFunc(gl::LESS);
    if (state == Depth_Compare::LessEqual)    gl::DepthFunc(gl::LEQUAL);
    if (state == Depth_Compare::Greater)      gl::DepthFunc(gl::GREATER);
    if (state == Depth_Compare::GreaterEqual) gl::DepthFunc(gl::GEQUAL);
    if (state == Depth_Compare::Equal)        gl::DepthFunc(gl::EQUAL);
    if (state == Depth_Compare::NotEqual)     gl::DepthFunc(gl::NOTEQUAL);
}

//============================================================================//

void Context::set_Scissor_Params(Scissor_Params args)
{
    if (impl_check_modify(crnt_Scissor_Params, args)) return;

    gl::Scissor(int(args.x), int(args.y), int(args.width), int(args.height));
}

void Context::set_Stencil_Params(Stencil_Params args)
{
    if (impl_check_modify(crnt_Stencil_Params, args)) return;

    gl::StencilFunc(args.func, args.ref, args.readMask);
    gl::StencilMask(args.writeMask);
}

void Context::set_Stencil_Custom(Stencil_Custom args)
{
    if (impl_check_modify(crnt_Stencil_Custom, args)) return;

    gl::StencilOp(args.fail, args.zFail, args.zPass);
}

void Context::set_Blend_Custom(Blend_Custom args)
{
    if (impl_check_modify(crnt_Blend_Custom, args)) return;

    gl::BlendFunc(args.srcFactor, args.destFactor);
}

//============================================================================//

void Context::set_ViewPort(Vec2U size)
{
    if (impl_check_modify(crnt_ViewPort, size)) return;

    gl::Viewport(0, 0, int(size.x), int(size.y));
}

//============================================================================//

void Context::bind_FrameBuffer_Read(const FrameBuffer& fbo)
{
    if (impl_check_modify(mFrameBufferBindingRead, &fbo)) return;

    gl::BindFramebuffer(gl::READ_FRAMEBUFFER, fbo.get_handle());
}

void Context::bind_FrameBuffer_Draw(const FrameBuffer& fbo)
{
    if (impl_check_modify(mFrameBufferBindingDraw, &fbo)) return;

    gl::BindFramebuffer(gl::DRAW_FRAMEBUFFER, fbo.get_handle());
}

void Context::bind_FrameBuffer(const FrameBuffer& fbo)
{
    if (!impl_check_modify(mFrameBufferBindingRead, &fbo))
        gl::BindFramebuffer(gl::READ_FRAMEBUFFER, fbo.get_handle());

    if (!impl_check_modify(mFrameBufferBindingDraw, &fbo))
        gl::BindFramebuffer(gl::DRAW_FRAMEBUFFER, fbo.get_handle());
}

void Context::bind_FrameBuffer_default()
{
    mFrameBufferBindingRead = nullptr;
    mFrameBufferBindingDraw = nullptr;

    gl::BindFramebuffer(gl::FRAMEBUFFER, 0u);
}

//============================================================================//

void Context::bind_Program(const Program& program)
{
    if (impl_check_modify(mProgramBinding, &program)) return;

    gl::UseProgram(program.get_handle());
}

void Context::bind_Program_default()
{
    mProgramBinding = nullptr;

    gl::UseProgram(0u);
}

//============================================================================//

void Context::bind_VertexArray(const VertexArray& vao)
{
    if (impl_check_modify(mVertexArrayBinding, &vao)) return;

    gl::BindVertexArray(vao.get_handle());
}

void Context::bind_VertexArray_dummy()
{
    mVertexArrayBinding = nullptr;

    gl::BindVertexArray(mDummyVAO);
}

//============================================================================//

void Context::bind_Texture(const Texture& tex, uint8_t slot)
{
    if (impl_check_modify(mTextureBindings[slot], &tex)) return;

    gl::ActiveTexture(gl::TEXTURE0 + slot);
    gl::BindTexture(tex.get_target(), tex.get_handle());
}

//============================================================================//

void Context::bind_UniformBuffer(const UniformBuffer& ubo, uint8_t index)
{
    const UniformBufferBinding binding { &ubo, 0u, 0u };
    if (impl_check_modify(mUniformBufferBindings[index], binding)) return;

    gl::BindBufferBase(gl::UNIFORM_BUFFER, index, ubo.get_handle());
}

void Context::bind_UniformBuffer(const UniformBuffer& ubo, uint8_t index, uint offset, uint size)
{
    const UniformBufferBinding binding { &ubo, offset, size };
    if (impl_check_modify(mUniformBufferBindings[index], binding)) return;

    gl::BindBufferRange(gl::UNIFORM_BUFFER, index, ubo.get_handle(), offset, size);
}

//============================================================================//

void Context::activate_PipelineState(const PipelineState& state)
{
    if (state.clip_distance) set_state(state.clip_distance.value());
    if (state.depth_clamp)   set_state(state.depth_clamp.value());
    if (state.cull_face)     set_state(state.cull_face.value());
    if (state.depth_test)    set_state(state.depth_test.value());
    if (state.stencil_test)  set_state(state.stencil_test.value());
    if (state.blend_mode)    set_state(state.blend_mode.value());
    if (state.depth_compare) set_state(state.depth_compare.value());

    if (state.scissor_params) set_Scissor_Params(state.scissor_params.value());
}

//============================================================================//

void Context::clear_Colour(Vec4F colour)
{
    gl::ClearColor(colour.r, colour.g, colour.b, colour.a);
    gl::Clear(gl::COLOR_BUFFER_BIT);
}

void Context::clear_Depth(double depth)
{
    if (crnt_Depth_Test != Depth_Test::Replace)
        gl::DepthMask(true);

    gl::ClearDepth(depth);
    gl::Clear(gl::DEPTH_BUFFER_BIT);

    if (crnt_Depth_Test == Depth_Test::Keep)
        gl::DepthMask(false);
}

void Context::clear_Stencil(uchar stencil, uchar mask)
{
    if (crnt_Stencil_Params.writeMask != mask)
        gl::StencilMask(mask);

    gl::ClearStencil(stencil);
    gl::Clear(gl::STENCIL_BUFFER_BIT);

    if (crnt_Stencil_Params.writeMask != mask)
        gl::StencilMask(crnt_Stencil_Params.writeMask);
}

void Context::clear_Depth_Stencil()
{
    if (crnt_Depth_Test != Depth_Test::Replace)
        gl::DepthMask(true);

    if (crnt_Stencil_Params.writeMask != 255u)
        gl::StencilMask(255u);

    gl::ClearDepth(1.0); gl::ClearStencil(0u);
    gl::Clear(gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT);

    if (crnt_Depth_Test == Depth_Test::Keep)
        gl::DepthMask(false);

    if (crnt_Stencil_Params.writeMask != 255u)
        gl::StencilMask(crnt_Stencil_Params.writeMask);
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
