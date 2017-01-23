#pragma once

#include <sqee/redist/gl_ext_4_2.hpp>

#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Shaders.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/VertexArray.hpp>

namespace sq {

//============================================================================//

/// OpenGL Context Interface
class Context final : NonCopyable
{
public:

    //========================================================//

    enum class Clip_Distance { Disable, Enable };
    enum class Depth_Clamp   { Disable, Enable };
    enum class Cull_Face     { Disable, Back, Front };
    enum class Depth_Test    { Disable, Keep, Replace };
    enum class Stencil_Test  { Disable, Keep, Replace, Custom };
    enum class Blend_Mode    { Disable, Accumulate, Alpha, PremAlpha, Custom };
    enum class Depth_Compare { Less, LessEqual, Greater, GreaterEqual, Equal, NotEqual };

    //========================================================//

    void set_state(Clip_Distance state)
    {
        if (impl_check_modify(crnt_Clip_Distance, state)) return;

        if (state == Clip_Distance::Disable) { gl::Disable(gl::CLIP_DISTANCE0); }
        if (state == Clip_Distance::Enable) { gl::Enable(gl::CLIP_DISTANCE0); }
    }

    //========================================================//

    void set_state(Depth_Clamp state)
    {
        if (impl_check_modify(crnt_Depth_Clamp, state)) return;

        if (state == Depth_Clamp::Disable) { gl::Disable(gl::DEPTH_CLAMP); }
        if (state == Depth_Clamp::Enable) { gl::Enable(gl::DEPTH_CLAMP); }
    }

    //========================================================//

    void set_state(Cull_Face state)
    {
        if (impl_check_modify(crnt_Cull_Face, state)) return;

        if (state == Cull_Face::Disable) { gl::Disable(gl::CULL_FACE); }
        if (state == Cull_Face::Back) { gl::Enable(gl::CULL_FACE); gl::CullFace(gl::BACK); }
        if (state == Cull_Face::Front) { gl::Enable(gl::CULL_FACE); gl::CullFace(gl::FRONT); }
    }

    //========================================================//

    void set_state(Depth_Test state)
    {
        if (impl_check_modify(crnt_Depth_Test, state)) return;

        if (state == Depth_Test::Disable) { gl::Disable(gl::DEPTH_TEST); }
        if (state == Depth_Test::Keep) { gl::Enable(gl::DEPTH_TEST); gl::DepthMask(false); }
        if (state == Depth_Test::Replace) { gl::Enable(gl::DEPTH_TEST); gl::DepthMask(true); }
    }

    //========================================================//

    void set_state(Stencil_Test state)
    {
        if (impl_check_modify(crnt_Stencil_Test, state)) return;

        if (state == Stencil_Test::Disable) { gl::Disable(gl::STENCIL_TEST); }
        if (state == Stencil_Test::Keep) { gl::Enable(gl::STENCIL_TEST); set_Stencil_Custom(gl::KEEP, gl::KEEP, gl::KEEP); }
        if (state == Stencil_Test::Replace) { gl::Enable(gl::STENCIL_TEST); set_Stencil_Custom(gl::KEEP, gl::KEEP, gl::REPLACE); }
        if (state == Stencil_Test::Custom) { gl::Enable(gl::STENCIL_TEST); }
    }

    //========================================================//

    void set_state(Blend_Mode state)
    {
        if (impl_check_modify(crnt_Blend_Mode, state)) return;

        if (state == Blend_Mode::Disable) { gl::Disable(gl::BLEND); }
        if (state == Blend_Mode::Accumulate) { gl::Enable(gl::BLEND); set_Blend_Custom(gl::ONE, gl::ONE); }
        if (state == Blend_Mode::Alpha) { gl::Enable(gl::BLEND); set_Blend_Custom(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA); }
        if (state == Blend_Mode::PremAlpha) { gl::Enable(gl::BLEND); set_Blend_Custom(gl::ONE, gl::ONE_MINUS_SRC_ALPHA); }
        if (state == Blend_Mode::Custom) { gl::Enable(gl::BLEND); }
    }

    //========================================================//

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

    //========================================================//

    void set_Stencil_Params(GLenum func, uchar ref, uchar readMask, uchar writeMask)
    {
        const Vec4U params { func, ref, readMask, writeMask };
        if (impl_check_modify(crnt_Stencil_Params, params)) return;

        gl::StencilFunc(func, ref, readMask);
        gl::StencilMask(writeMask);
    }

    //========================================================//

    void set_Stencil_Custom(GLenum fail, GLenum zFail, GLenum zPass)
    {
        const Vec3U params { fail, zFail, zPass };
        if (impl_check_modify(crnt_Stencil_Custom, params)) return;

        gl::StencilOp(fail, zFail, zPass);
    }

    //========================================================//

    void set_Blend_Custom(GLenum srcFactor, GLenum destFactor)
    {
        const Vec2U params { srcFactor, destFactor };
        if (impl_check_modify(crnt_Blend_Custom, params)) return;

        gl::BlendFunc(srcFactor, destFactor);
    }

    //========================================================//

    // FrameBuffer bind methods /////

    void bind_FrameBuffer_Read(const FrameBuffer& fbo)
    {
        if (impl_check_modify(crnt_FrameBuffer_Read, &fbo)) return;

        gl::BindFramebuffer(gl::READ_FRAMEBUFFER, fbo.get_handle());
    }

    void bind_FrameBuffer_Draw(const FrameBuffer& fbo)
    {
        if (impl_check_modify(crnt_FrameBuffer_Draw, &fbo)) return;

        gl::BindFramebuffer(gl::DRAW_FRAMEBUFFER, fbo.get_handle());
    }

    void bind_FrameBuffer(const FrameBuffer& fbo)
    {
        bind_FrameBuffer_Read(fbo);
        bind_FrameBuffer_Draw(fbo);
    }

    void bind_FrameBuffer_default()
    {
        gl::BindFramebuffer(gl::FRAMEBUFFER, 0u);
        crnt_FrameBuffer_Read = nullptr;
        crnt_FrameBuffer_Draw = nullptr;
    }

    //========================================================//

    // Texture bind methods /////

    void bind_Texture(const Texture& tex, uint slot)
    {
        if (impl_check_modify(crnt_Textures[slot], &tex)) return;

        gl::ActiveTexture(gl::TEXTURE0 + slot);
        gl::BindTexture(tex.get_target(), tex.get_handle());
    }

    //========================================================//

    // UniformBuffer bind methods /////

    void bind_UniformBuffer(const UniformBuffer& ubo, uint index)
    {
        if (impl_check_modify(crnt_UniformBuffers[index], &ubo)) return;

        gl::BindBufferBase(gl::UNIFORM_BUFFER, index, ubo.get_handle());
    }

    //========================================================//

    // VertexArray bind methods /////

    void bind_VertexArray(const VertexArray& vao)
    {
        if (impl_check_modify(crnt_VertexArray, &vao)) return;

        gl::BindVertexArray(vao.get_handle());
    }

    void bind_VertexArray_dummy()
    {
        gl::BindVertexArray(mDummyVAO);
        crnt_VertexArray = nullptr;
    }

    //========================================================//

    // Shader use methods /////

    void use_Shader_Vert(const Shader& shader)
    {
        if (impl_check_modify(crnt_Shader_Vert, &shader)) return;

        gl::UseProgramStages(mPipeline, gl::VERTEX_SHADER_BIT, shader.get_handle());
    }

    void use_Shader_Geom(const Shader& shader)
    {
        if (impl_check_modify(crnt_Shader_Geom, &shader)) return;

        gl::UseProgramStages(mPipeline, gl::GEOMETRY_SHADER_BIT, shader.get_handle());
    }

    void use_Shader_Frag(const Shader& shader)
    {
        if (impl_check_modify(crnt_Shader_Frag, &shader)) return;

        gl::UseProgramStages(mPipeline, gl::FRAGMENT_SHADER_BIT, shader.get_handle());
    }

    //========================================================//

    // shader pipeline methods /////

    void disable_shader_stage_geometry()
    {
        gl::UseProgramStages(mPipeline, gl::GEOMETRY_SHADER_BIT, 0u);
        crnt_Shader_Geom = nullptr;
    }

    void disable_shader_stage_fragment()
    {
        gl::UseProgramStages(mPipeline, gl::FRAGMENT_SHADER_BIT, 0u);
        crnt_Shader_Frag = nullptr;
    }

    void bind_shader_pipeline()
    {
        gl::BindProgramPipeline(mPipeline);
    }

    //========================================================//

    // ViewPort set, get methods /////

    void set_ViewPort(Vec2U size)
    {
        if (impl_check_modify(crnt_ViewPort, size)) return;

        gl::Viewport(0, 0, int(size.x), int(size.y));
    }

    Vec2U get_ViewPort() const
    {
        return crnt_ViewPort;
    }

    //========================================================//

    // FrameBuffer clear methods /////

    void clear_Colour(Vec4F colour);

    void clear_Depth(double depth);

    void clear_Stencil(uchar stencil, uchar mask);

    void clear_Depth_Stencil();

    //========================================================//

    /// Get the Static Instance
    static Context& get();

private:

    //========================================================//

    Clip_Distance crnt_Clip_Distance = Clip_Distance ::Disable;
    Depth_Clamp   crnt_Depth_Clamp   = Depth_Clamp   ::Disable;
    Cull_Face     crnt_Cull_Face     = Cull_Face     ::Disable;
    Depth_Test    crnt_Depth_Test    = Depth_Test    ::Disable;
    Stencil_Test  crnt_Stencil_Test  = Stencil_Test  ::Disable;
    Blend_Mode    crnt_Blend_Mode    = Blend_Mode    ::Disable;
    Depth_Compare crnt_Depth_Compare = Depth_Compare ::Less;

    Vec4U crnt_Stencil_Params = Vec4U(gl::ALWAYS, 0u, 255u, 255u);
    Vec3U crnt_Stencil_Custom = Vec3U(gl::KEEP, gl::KEEP, gl::KEEP);
    Vec2U crnt_Blend_Custom = Vec2U(gl::ONE, gl::ZERO);

    Vec2U crnt_ViewPort = Vec2U(0u, 0u);

    //========================================================//

    const FrameBuffer* crnt_FrameBuffer_Read = nullptr;
    const FrameBuffer* crnt_FrameBuffer_Draw = nullptr;

    const Shader* crnt_Shader_Vert = nullptr;
    const Shader* crnt_Shader_Geom = nullptr;
    const Shader* crnt_Shader_Frag = nullptr;

    const Texture* crnt_Textures[16] = {nullptr};

    const UniformBuffer* crnt_UniformBuffers[8] = {nullptr};

    const VertexArray* crnt_VertexArray = nullptr;

    //========================================================//

    template <class T>
    bool impl_check_modify(T& ref, T value)
    {
        if (ref == value) return true;
        ref = value; return false;
    }

    //========================================================//

    void impl_reset_FrameBuffer(const FrameBuffer* old, const FrameBuffer* fresh = nullptr)
    {
        if (crnt_FrameBuffer_Read == old) crnt_FrameBuffer_Read = fresh;
        if (crnt_FrameBuffer_Draw == old) crnt_FrameBuffer_Draw = fresh;
    }

    void impl_reset_Shader(const Shader* old, const Shader* fresh = nullptr)
    {
        if (crnt_Shader_Vert == old) crnt_Shader_Vert = fresh;
        if (crnt_Shader_Geom == old) crnt_Shader_Geom = fresh;
        if (crnt_Shader_Frag == old) crnt_Shader_Frag = fresh;
    }

    void impl_reset_Texture(const Texture* old, const Texture* fresh = nullptr)
    {
        for (auto& ref : crnt_Textures) if (ref == old) ref = fresh;
    }

    void impl_reset_UniformBuffer(const UniformBuffer* old, const UniformBuffer* fresh = nullptr)
    {
        for (auto& ref : crnt_UniformBuffers) if (ref == old) ref = fresh;
    }

    void impl_reset_VertexArray(const VertexArray* old, const VertexArray* fresh = nullptr)
    {
        if (crnt_VertexArray == old) crnt_VertexArray = fresh;
    }

    //========================================================//

    Context();
    ~Context();

    GLenum mPipeline = 0u;
    GLenum mDummyVAO = 0u;

    friend class FrameBuffer;
    friend class Shader;
    friend class Texture;
    friend class UniformBuffer;
    friend class VertexArray;
};

//============================================================================//

} // namespace sq
