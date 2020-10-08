// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>
#include <sqee/gl/SafeEnums.hpp>

namespace sq {

//====== Forward Declarations ================================================//

class FixedBuffer;
class FrameBuffer;
class Program;
class Texture;
class VertexArray;

//============================================================================//

/// OpenGL Context Interface.
class SQEE_API Context final : private NonCopyable
{
public: //====================================================//

    /// Get the static instance,
    static Context& get();

    //--------------------------------------------------------//

    /// Update AlphaCoverage context state.
    void set_state(AlphaCoverage state);

    /// Update ClipDistance context state.
    void set_state(ClipDistance state);

    /// Update DepthClamp context state.
    void set_state(DepthClamp state);

    /// Update ScissorTest context state.
    void set_state(ScissorTest state);

    //--------------------------------------------------------//

    /// Update BlendMode context state.
    void set_state(BlendMode state);

    /// Update CullFace context state.
    void set_state(CullFace state);

    /// Update DepthTest context state.
    void set_state(DepthTest state);

    /// Update StencilTest context state.
    void set_state(StencilTest state);

    //--------------------------------------------------------//

    /// Set the box to use for scissor testing.
    void set_scissor_box(uint x, uint y, uint width, uint height);

    /// Set compare function to use for depth testing.
    void set_depth_compare(CompareFunc func);

    /// Set parameters to use for stencil testing.
    void set_stencil_params(CompareFunc func, uchar ref, uchar readMask, uchar writeMask);

    /// Set operations to use for custom stencil mode.
    void set_stencil_custom(StencilOp fail, StencilOp zFail, StencilOp zPass);

    /// Set factors to use for custom blend mode.
    void set_blend_custom(BlendFactor src, BlendFactor dest);

    //--------------------------------------------------------//

    /// Set the width used for drawing lines.
    void set_line_width(float width);

    //--------------------------------------------------------//

    /// Set the size of the drawing viewport.
    void set_ViewPort(Vec2U size);

    /// Get the size of the drawing viewport.
    Vec2U get_ViewPort() const { return mCurrentViewPort; }

    //--------------------------------------------------------//

    /// Bind a Buffer object to an indexed target.
    void bind_buffer(const FixedBuffer& buffer, BufTarget target, uint slot);

    /// Bind part of a Buffer object to an indexed target.
    void bind_buffer(const FixedBuffer& buffer, BufTarget target, uint slot, uint offset, uint size);

    /// Bind a FrameBuffer object.
    void bind_framebuffer(const FrameBuffer& fbo, FboTarget target = FboTarget::Both);

    /// Bind a Program object.
    void bind_program(const Program& program);

    /// Bind a Texture object to a slot.
    void bind_texture(const Texture& tex, uint slot);

    /// Bind a VertexArray object.
    void bind_vertexarray(const VertexArray& vao);

    //--------------------------------------------------------//

    /// Bind the default FrameBuffer object.
    void bind_framebuffer_default(FboTarget target = FboTarget::Both);

    /// Bind the default Program object.
    void bind_program_default();

    /// Bind the dummy VertexArray object.
    void bind_vertexarray_dummy();

    //--------------------------------------------------------//

    /// Clear the bound depth buffer.
    void clear_depth(double depth);

    /// Clear the bound stencil buffer.
    void clear_stencil(uchar stencil, uchar mask);

    /// Clear the bound colour buffer.
    void clear_colour(Vec4F colour);

    /// Clear the bound depth and stencil buffers.
    void clear_depth_stencil(double depth, uchar stencil, uchar mask);

    /// Clear the bound depth and colour buffers.
    void clear_depth_colour(double depth, Vec4F colour);

    /// Clear the bound stencil and colour buffers.
    void clear_stencil_colour(uchar stencil, uchar mask, Vec4F colour);

    /// Clear the bound depth, stencil, and colour buffers.
    void clear_depth_stencil_colour(double depth, uchar stencil, uchar mask, Vec4F colour);

    //--------------------------------------------------------//

    /// Simple wrapper function for glDrawArrays.
    static void draw_arrays(DrawPrimitive primitive, uint first, uint count);

    /// Wrapper for glDrawElements taking one-byte indices.
    static void draw_elements_u8(DrawPrimitive primitive, uint8_t first, uint8_t count);

    /// Wrapper for glDrawElements taking two-byte indices.
    static void draw_elements_u16(DrawPrimitive primitive, uint16_t first, uint16_t count);

    /// Wrapper for glDrawElements taking four-byte indices.
    static void draw_elements_u32(DrawPrimitive primitive, uint32_t first, uint32_t count);

private: //===================================================//

    Context();
    ~Context();

    Vec2U mCurrentViewPort = { 0u, 0u };

    GLuint mDummyVAO = 0u;
};

//============================================================================//

} // namespace sq
