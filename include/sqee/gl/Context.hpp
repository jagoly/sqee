#pragma once

#include <sqee/redist/gl_loader.hpp>

#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Program.hpp>
#include <sqee/gl/TextureBase.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/VertexArray.hpp>

namespace sq {

//============================================================================//

/// OpenGL Context Interface.
class Context final : NonCopyable
{
public: //====================================================//

    enum class Clip_Distance  : bool { Disable, Enable };
    enum class Depth_Clamp    : bool { Disable, Enable };
//    enum class Alpha_Coverage : bool { Disable, Enable };
    enum class Cull_Face      : char { Disable, Back, Front };
    enum class Depth_Test     : char { Disable, Keep, Replace };
    enum class Stencil_Test   : char { Disable, Keep, Replace, Custom };
    enum class Blend_Mode     : char { Disable, Accumulate, Alpha, PremAlpha, Custom };
    enum class Depth_Compare  : char { Less, LessEqual, Greater, GreaterEqual, Equal, NotEqual };

    //--------------------------------------------------------//

    /// Update Clip_Distance context state.
    void set_state(Clip_Distance state)
    {
        if (impl_check_modify(crnt_Clip_Distance, state)) return;

        if (state == Clip_Distance::Disable) { gl::Disable(gl::CLIP_DISTANCE0); }
        if (state == Clip_Distance::Enable) { gl::Enable(gl::CLIP_DISTANCE0); }
    }

    /// Update Depth_Clamp context state.
    void set_state(Depth_Clamp state)
    {
        if (impl_check_modify(crnt_Depth_Clamp, state)) return;

        if (state == Depth_Clamp::Disable) { gl::Disable(gl::DEPTH_CLAMP); }
        if (state == Depth_Clamp::Enable) { gl::Enable(gl::DEPTH_CLAMP); }
    }

//    /// Update Alpha_Coverage context state.
//    void set_state(Alpha_Coverage state)
//    {
//        if (impl_check_modify(crnt_Alpha_Coverage, state)) return;

//        if (state == Alpha_Coverage::Disable) { gl::Disable(gl::SAMPLE_ALPHA_TO_COVERAGE); }
//        if (state == Alpha_Coverage::Enable) { gl::Enable(gl::SAMPLE_ALPHA_TO_COVERAGE); }
//    }

    /// Update Cull_Face context state.
    void set_state(Cull_Face state)
    {
        if (impl_check_modify(crnt_Cull_Face, state)) return;

        if (state == Cull_Face::Disable) { gl::Disable(gl::CULL_FACE); }
        if (state == Cull_Face::Back) { gl::Enable(gl::CULL_FACE); gl::CullFace(gl::BACK); }
        if (state == Cull_Face::Front) { gl::Enable(gl::CULL_FACE); gl::CullFace(gl::FRONT); }
    }

    /// Update Depth_Test context state.
    void set_state(Depth_Test state)
    {
        if (impl_check_modify(crnt_Depth_Test, state)) return;

        if (state == Depth_Test::Disable) { gl::Disable(gl::DEPTH_TEST); }
        if (state == Depth_Test::Keep) { gl::Enable(gl::DEPTH_TEST); gl::DepthMask(false); }
        if (state == Depth_Test::Replace) { gl::Enable(gl::DEPTH_TEST); gl::DepthMask(true); }
    }

    /// Update Stencil_Test context state.
    void set_state(Stencil_Test state)
    {
        if (impl_check_modify(crnt_Stencil_Test, state)) return;

        if (state == Stencil_Test::Disable) { gl::Disable(gl::STENCIL_TEST); }
        if (state == Stencil_Test::Keep) { gl::Enable(gl::STENCIL_TEST); set_Stencil_Custom(gl::KEEP, gl::KEEP, gl::KEEP); }
        if (state == Stencil_Test::Replace) { gl::Enable(gl::STENCIL_TEST); set_Stencil_Custom(gl::KEEP, gl::KEEP, gl::REPLACE); }
        if (state == Stencil_Test::Custom) { gl::Enable(gl::STENCIL_TEST); }
    }

    /// Update Blend_Mode context state.
    void set_state(Blend_Mode state)
    {
        if (impl_check_modify(crnt_Blend_Mode, state)) return;

        if (state == Blend_Mode::Disable) { gl::Disable(gl::BLEND); }
        if (state == Blend_Mode::Accumulate) { gl::Enable(gl::BLEND); set_Blend_Custom(gl::ONE, gl::ONE); }
        if (state == Blend_Mode::Alpha) { gl::Enable(gl::BLEND); set_Blend_Custom(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA); }
        if (state == Blend_Mode::PremAlpha) { gl::Enable(gl::BLEND); set_Blend_Custom(gl::ONE, gl::ONE_MINUS_SRC_ALPHA); }
        if (state == Blend_Mode::Custom) { gl::Enable(gl::BLEND); }
    }

    /// Update Depth_Compare context state.
    void set_state(Depth_Compare state)
    {
        if (impl_check_modify(crnt_Depth_Compare, state)) return;

        if (state == Depth_Compare::Less) { gl::DepthFunc(gl::LESS); }
        if (state == Depth_Compare::LessEqual) { gl::DepthFunc(gl::LEQUAL); }
        if (state == Depth_Compare::Greater) { gl::DepthFunc(gl::GREATER); }
        if (state == Depth_Compare::GreaterEqual) { gl::DepthFunc(gl::GEQUAL); }
        if (state == Depth_Compare::Equal) { gl::DepthFunc(gl::EQUAL); }
        if (state == Depth_Compare::NotEqual) { gl::DepthFunc(gl::NOTEQUAL); }
    }

    //--------------------------------------------------------//

    /// Set mask parameters to use for stencil testing.
    void set_Stencil_Params(GLenum func, uchar ref, uchar readMask, uchar writeMask)
    {
        const Vec4U params { func, ref, readMask, writeMask };
        if (impl_check_modify(crnt_Stencil_Params, params)) return;

        gl::StencilFunc(func, ref, readMask);
        gl::StencilMask(writeMask);
    }

    /// Set custom Stencil_Test state parameters.
    void set_Stencil_Custom(GLenum fail, GLenum zFail, GLenum zPass)
    {
        const Vec3U params { fail, zFail, zPass };
        if (impl_check_modify(crnt_Stencil_Custom, params)) return;

        gl::StencilOp(fail, zFail, zPass);
    }

    /// Set custom Blend_Mode state parameters.
    void set_Blend_Custom(GLenum srcFactor, GLenum destFactor)
    {
        const Vec2U params { srcFactor, destFactor };
        if (impl_check_modify(crnt_Blend_Custom, params)) return;

        gl::BlendFunc(srcFactor, destFactor);
    }

    //--------------------------------------------------------//

    /// Set the size of the drawing viewport.
    void set_ViewPort(Vec2U size)
    {
        if (impl_check_modify(crnt_ViewPort, size)) return;
        gl::Viewport(0, 0, int(size.x), int(size.y));
    }

    /// Get the size of the drawing viewport.
    Vec2U get_ViewPort() const
    {
        return crnt_ViewPort;
    }

    //--------------------------------------------------------//

    /// Bind a FrameBuffer for reading only.
    void bind_FrameBuffer_Read(const FrameBuffer& fbo)
    {
        if (impl_check_modify(mFrameBufferBindingRead, &fbo)) return;

        gl::BindFramebuffer(gl::READ_FRAMEBUFFER, fbo.get_handle());
    }

    /// Bind a FrameBuffer for drawing only.
    void bind_FrameBuffer_Draw(const FrameBuffer& fbo)
    {
        if (impl_check_modify(mFrameBufferBindingDraw, &fbo)) return;

        gl::BindFramebuffer(gl::DRAW_FRAMEBUFFER, fbo.get_handle());
    }

    /// Bind a FrameBuffer for read and draw.
    void bind_FrameBuffer(const FrameBuffer& fbo)
    {
        bind_FrameBuffer_Read(fbo);
        bind_FrameBuffer_Draw(fbo);
    }

    /// Bind the default FrameBuffer.
    void bind_FrameBuffer_default()
    {
        mFrameBufferBindingRead = nullptr;
        mFrameBufferBindingDraw = nullptr;

        gl::BindFramebuffer(gl::FRAMEBUFFER, 0u);
    }

    //--------------------------------------------------------//

    /// Bind the specified Program.
    void bind_Program(const Program& program)
    {
        if (impl_check_modify(mProgramBinding, &program)) return;

        gl::UseProgram(program.get_handle());
    }

    /// Bind the the default Program.
    void bind_Program_default()
    {
        mProgramBinding = nullptr;

        gl::UseProgram(0u);
    }

    //--------------------------------------------------------//

    /// Bind the specified VertexArray.
    void bind_VertexArray(const VertexArray& vao)
    {
        if (impl_check_modify(mVertexArrayBinding, &vao)) return;

        gl::BindVertexArray(vao.get_handle());
    }

    /// Bind the dummy VertexArray.
    void bind_VertexArray_dummy()
    {
        mVertexArrayBinding = nullptr;

        gl::BindVertexArray(mDummyVAO);
    }

    //--------------------------------------------------------//

    /// Bind a Texture to the specified slot.
    void bind_Texture(const Texture& tex, uint slot)
    {
        if (impl_check_modify(mTextureBindings[slot], &tex)) return;

        gl::ActiveTexture(gl::TEXTURE0 + slot);
        gl::BindTexture(tex.get_target(), tex.get_handle());
    }

    //--------------------------------------------------------//

    /// Bind a UniformBuffer to the specified index.
    void bind_UniformBuffer(const UniformBuffer& ubo, uint index)
    {
        const UniformBufferBinding binding { &ubo, 0u, 0u };
        if (impl_check_modify(mUniformBufferBindings[index], binding)) return;

        gl::BindBufferBase(gl::UNIFORM_BUFFER, index, ubo.get_handle());
    }

    /// Bind a portion of a UniformBuffer to the specified index.
    void bind_UniformBuffer(const UniformBuffer& ubo, uint index, uint offset, uint size)
    {
        const UniformBufferBinding binding { &ubo, offset, size };
        if (impl_check_modify(mUniformBufferBindings[index], binding)) return;

        gl::BindBufferRange(gl::UNIFORM_BUFFER, index, ubo.get_handle(), offset, size);
    }

    //--------------------------------------------------------//

    /// Clear the bound FrameBuffer's colour.
    void clear_Colour(Vec4F colour);

    /// Clear the bound FrameBuffer's depth.
    void clear_Depth(double depth);

    /// Clear the bound FrameBuffer's stencil.
    void clear_Stencil(uchar stencil, uchar mask);

    /// Clear the bound FrameBuffer's depth and stencil.
    void clear_Depth_Stencil();

    //--------------------------------------------------------//

    /// Get the static instance,
    static Context& get();

private: //===================================================//

    Clip_Distance  crnt_Clip_Distance  = Clip_Distance  :: Disable;
    Depth_Clamp    crnt_Depth_Clamp    = Depth_Clamp    :: Disable;
//    Alpha_Coverage crnt_Alpha_Coverage = Alpha_Coverage :: Disable;
    Cull_Face      crnt_Cull_Face      = Cull_Face      :: Disable;
    Depth_Test     crnt_Depth_Test     = Depth_Test     :: Disable;
    Stencil_Test   crnt_Stencil_Test   = Stencil_Test   :: Disable;
    Blend_Mode     crnt_Blend_Mode     = Blend_Mode     :: Disable;
    Depth_Compare  crnt_Depth_Compare  = Depth_Compare  :: Less;

    Vec4U crnt_Stencil_Params = { gl::ALWAYS, 0u, 255u, 255u };
    Vec3U crnt_Stencil_Custom = { gl::KEEP, gl::KEEP, gl::KEEP };
    Vec2U crnt_Blend_Custom = { gl::ONE, gl::ZERO };

    Vec2U crnt_ViewPort = { 0u, 0u };

    //--------------------------------------------------------//

    const FrameBuffer* mFrameBufferBindingRead = nullptr;
    const FrameBuffer* mFrameBufferBindingDraw = nullptr;

    const Program* mProgramBinding = nullptr;

    const VertexArray* mVertexArrayBinding = nullptr;

    const Texture* mTextureBindings[12] { nullptr };

    //--------------------------------------------------------//

    struct UniformBufferBinding
    {
        const UniformBuffer* ptr = nullptr;
        uint offset = 0u, size = 0u;

        bool operator==(const UniformBufferBinding& rhs) const
        { return ptr == rhs.ptr && offset == rhs.offset && size == rhs.size; }
    };

    UniformBufferBinding mUniformBufferBindings[6];

    //--------------------------------------------------------//

    template <class T>
    bool impl_check_modify(T& ref, T value)
    {
        if (ref == value) return true;
        ref = value; return false;
    }

    //--------------------------------------------------------//

    Context();
    ~Context();

    //--------------------------------------------------------//

    GLuint mDummyVAO = 0u;

    //--------------------------------------------------------//

    void impl_reset_FrameBuffer(const FrameBuffer* old, const FrameBuffer* fresh = nullptr);
    void impl_reset_Program(const Program* old, const Program* fresh = nullptr);
    void impl_reset_VertexArray(const VertexArray* old, const VertexArray* fresh = nullptr);
    void impl_reset_Texture(const Texture* old, const Texture* fresh = nullptr);
    void impl_reset_UniformBuffer(const UniformBuffer* old, const UniformBuffer* fresh = nullptr);

    //--------------------------------------------------------//

    friend class FrameBuffer;
    friend class Program;
    friend class VertexArray;
    friend class Texture;
    friend class UniformBuffer;
};

//============================================================================//

} // namespace sq
