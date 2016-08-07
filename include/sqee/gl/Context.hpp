#pragma once

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/Vectors.hpp>

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>

#include <sqee/assert.hpp>

namespace sq {

class Context final : NonCopyable {
public:
    enum class Clip_Distance { Disable, Enable };
    enum class Depth_Clamp   { Disable, Enable };
    enum class Cull_Face     { Disable, Back, Front };
    enum class Depth_Test    { Disable, Keep, Replace };
    enum class Stencil_Test  { Disable, Keep, Replace, Custom };
    enum class Blend_Mode    { Disable, Accumulate, Alpha, PremAlpha, Custom };
    enum class Depth_Compare { Less, LessEqual, Greater, GreaterEqual, Equal, NotEqual };

    void set_state(Clip_Distance _state) {
        if (impl_check_modify(crnt_Clip_Distance, _state)) {
            if (_state == Clip_Distance::Disable) { gl::Disable(gl::CLIP_DISTANCE0); }
            if (_state == Clip_Distance::Enable) { gl::Enable(gl::CLIP_DISTANCE0); }
    }}

    void set_state(Depth_Clamp _state) {
        if (impl_check_modify(crnt_Depth_Clamp, _state)) {
            if (_state == Depth_Clamp::Disable) { gl::Disable(gl::DEPTH_CLAMP); }
            if (_state == Depth_Clamp::Enable) { gl::Enable(gl::DEPTH_CLAMP); }
    }}

    void set_state(Cull_Face _state) {
        if (impl_check_modify(crnt_Cull_Face, _state)) {
            if (_state == Cull_Face::Disable) { gl::Disable(gl::CULL_FACE); }
            if (_state == Cull_Face::Back) { gl::Enable(gl::CULL_FACE); gl::CullFace(gl::BACK); }
            if (_state == Cull_Face::Front) { gl::Enable(gl::CULL_FACE); gl::CullFace(gl::FRONT); }
    }}

    void set_state(Depth_Test _state) {
        if (impl_check_modify(crnt_Depth_Test, _state)) {
            if (_state == Depth_Test::Disable) { gl::Disable(gl::DEPTH_TEST); }
            if (_state == Depth_Test::Keep) { gl::Enable(gl::DEPTH_TEST); gl::DepthMask(false); }
            if (_state == Depth_Test::Replace) { gl::Enable(gl::DEPTH_TEST); gl::DepthMask(true); }
    }}

    void set_state(Stencil_Test _state) {
        if (impl_check_modify(crnt_Stencil_Test, _state)) {
            if (_state == Stencil_Test::Disable) { gl::Disable(gl::STENCIL_TEST); }
            if (_state == Stencil_Test::Keep) { gl::Enable(gl::STENCIL_TEST); set_Stencil_Custom(gl::KEEP, gl::KEEP, gl::KEEP); }
            if (_state == Stencil_Test::Replace) { gl::Enable(gl::STENCIL_TEST); set_Stencil_Custom(gl::KEEP, gl::KEEP, gl::REPLACE); }
            if (_state == Stencil_Test::Custom) { gl::Enable(gl::STENCIL_TEST); }
    }}

    void set_state(Blend_Mode _state) {
        if (impl_check_modify(crnt_Blend_Mode, _state)) {
            if (_state == Blend_Mode::Disable) { gl::Disable(gl::BLEND); }
            if (_state == Blend_Mode::Accumulate) { gl::Enable(gl::BLEND); set_Blend_Custom(gl::ONE, gl::ONE); }
            if (_state == Blend_Mode::Alpha) { gl::Enable(gl::BLEND); set_Blend_Custom(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA); }
            if (_state == Blend_Mode::PremAlpha) { gl::Enable(gl::BLEND); set_Blend_Custom(gl::ONE, gl::ONE_MINUS_SRC_ALPHA); }
            if (_state == Blend_Mode::Custom) { gl::Enable(gl::BLEND); }
    }}

    void set_state(Depth_Compare _state) {
        if (impl_check_modify(crnt_Depth_Compare, _state)) {
            if (_state == Depth_Compare::Less) { gl::DepthFunc(gl::LESS); }
            if (_state == Depth_Compare::LessEqual) { gl::DepthFunc(gl::LEQUAL); }
            if (_state == Depth_Compare::Greater) { gl::DepthFunc(gl::GREATER); }
            if (_state == Depth_Compare::GreaterEqual) { gl::DepthFunc(gl::GEQUAL); }
            if (_state == Depth_Compare::Equal) { gl::DepthFunc(gl::EQUAL); }
            if (_state == Depth_Compare::NotEqual) { gl::DepthFunc(gl::NOTEQUAL); }
    }}


    void set_Stencil_Params(GLenum _func, uchar _ref, uchar _readMask, uchar _writeMask) {
        if (impl_check_modify(crnt_Stencil_Params, {_func, _ref, _readMask, _writeMask})) {
            gl::StencilFunc(_func, _ref, _readMask); gl::StencilMask(_writeMask); }
    }

    void set_Stencil_Custom(GLenum _fail, GLenum _zFail, GLenum _zPass) {
        if (impl_check_modify(crnt_Stencil_Custom, {_fail, _zFail, _zPass}))
            gl::StencilOp(_fail, _zFail, _zPass);
    }

    void set_Blend_Custom(GLenum _sFactor, GLenum _dFactor) {
        if (impl_check_modify(crnt_Blend_Custom, {_sFactor, _dFactor}))
            gl::BlendFunc(_sFactor, _dFactor);
    }


    // UniformBuffer bind methods /////

    void bind_UniformBuffer(const UniformBuffer& _ubo, uint _index) {
        if (impl_check_modify(crnt_UniformBuffers[_index], &_ubo))
            gl::BindBufferBase(gl::UNIFORM_BUFFER, _index, _ubo.get_handle());
    }


    // FrameBuffer bind methods /////

    void bind_read_FrameBuffer(const FrameBuffer& _fbo) {
        if (impl_check_modify(crnt_read_FrameBuffer, &_fbo))
            gl::BindFramebuffer(gl::READ_FRAMEBUFFER, _fbo.get_handle());
    }

    void bind_draw_FrameBuffer(const FrameBuffer& _fbo) {
        if (impl_check_modify(crnt_draw_FrameBuffer, &_fbo))
            gl::BindFramebuffer(gl::DRAW_FRAMEBUFFER, _fbo.get_handle());
    }

    void bind_FrameBuffer(const FrameBuffer& _fbo) {
        bind_read_FrameBuffer(_fbo); bind_draw_FrameBuffer(_fbo);
    }


    // Texture bind methods /////

    void bind_Texture(const Texture& _tex, uint _slot) {
        if (impl_check_modify(crnt_Textures[_slot], &_tex)) {
            gl::ActiveTexture(gl::TEXTURE0 + _slot);
            gl::BindTexture(_tex.get_target(), _tex.get_handle());
    }}


    // ViewPort set methods /////

    void set_ViewPort(Vec2U _size) {
        if (impl_check_modify(crnt_ViewPort, _size))
            gl::Viewport(0, 0, _size.x, _size.y);
    }


    // FrameBuffer clear methods /////

    void clear_Colour(Vec4F _colour) {
        gl::ClearColor(_colour.r, _colour.g, _colour.b, _colour.a);
        gl::Clear(gl::COLOR_BUFFER_BIT);
    }

    void clear_Depth(double _depth) {
        if (crnt_Depth_Test != Depth_Test::Replace) gl::DepthMask(true);
        gl::ClearDepth(_depth); gl::Clear(gl::DEPTH_BUFFER_BIT);
        if (crnt_Depth_Test == Depth_Test::Keep) gl::DepthMask(false);
    }

    void clear_Stencil(uchar _stencil, uchar _mask) {
        if (crnt_Stencil_Params.w != _mask) gl::StencilMask(_mask);
        gl::ClearStencil(_stencil); gl::Clear(gl::STENCIL_BUFFER_BIT);
        if (crnt_Stencil_Params.w != _mask) gl::StencilMask(crnt_Stencil_Params.w);
    }

    void clear_Depth_Stencil() {
        if (crnt_Depth_Test != Depth_Test::Replace) gl::DepthMask(true);
        if (crnt_Stencil_Params.w != 255u) gl::StencilMask(255u);
        gl::ClearDepth(1.0); gl::ClearStencil(0u);
        gl::Clear(gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT);
        if (crnt_Depth_Test == Depth_Test::Keep) gl::DepthMask(false);
        if (crnt_Stencil_Params.w != 255u) gl::StencilMask(crnt_Stencil_Params.w);
    }

    void clear_Colour_Depth_Stencil() {
        clear_Colour(Vec4F(0.f));
        clear_Depth_Stencil();
    }

    /// Get the Static Instance
    static Context& get();

private:
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

    const UniformBuffer* crnt_UniformBuffers[10] = {nullptr};
    const FrameBuffer* crnt_read_FrameBuffer = nullptr;
    const FrameBuffer* crnt_draw_FrameBuffer = nullptr;
    const Texture* crnt_Textures[10] = {nullptr};

    template<class T> bool impl_check_modify(T& _ref, T _value) {
        if (_ref != _value) { _ref = _value; return true; }
        return false;
    }

    void impl_delete_UniformBuffer(const UniformBuffer* _ubo) {
        for (auto& ref : crnt_UniformBuffers) if (ref == _ubo) ref = nullptr;
    }

    void impl_delete_FrameBuffer(const FrameBuffer* _fbo) {
        if (crnt_read_FrameBuffer == _fbo) crnt_read_FrameBuffer = nullptr;
        if (crnt_draw_FrameBuffer == _fbo) crnt_draw_FrameBuffer = nullptr;
    }

    void impl_delete_Texture(const Texture* _tex) {
        for (auto& ref : crnt_Textures) if (ref == _tex) ref = nullptr;
    }

    /// Private Constructor
    Context() = default;

    friend class UniformBuffer;
    friend class FrameBuffer;
    friend class Texture;
};

}
