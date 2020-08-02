// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

namespace sq {

//====== Forward Declarations ================================================//

class FrameBuffer;
class Program;
class Texture;
class UniformBuffer;
class VertexArray;

//============================================================================//

/// OpenGL Context Interface.
class SQEE_API Context final : private NonCopyable
{
public: //====================================================//

    enum class Clip_Distance  : bool { Disable, Enable };
    enum class Depth_Clamp    : bool { Disable, Enable };
    enum class Scissor_Test   : bool { Disable, Enable };
//    enum class Alpha_Coverage : bool { Disable, Enable };
    enum class Cull_Face      : char { Disable, Back, Front };
    enum class Depth_Test     : char { Disable, Keep, Replace };
    enum class Stencil_Test   : char { Disable, Keep, Replace, Custom };
    enum class Blend_Mode     : char { Disable, Accumulate, Alpha, PremAlpha, Custom };
    enum class Depth_Compare  : char { Less, LessEqual, Greater, GreaterEqual, Equal, NotEqual };

    //--------------------------------------------------------//

    struct Scissor_Params { uint x; uint y; uint width; uint height; };
    struct Stencil_Params { GLenum func; uchar ref; uchar readMask; uchar writeMask; };
    struct Stencil_Custom { GLenum fail; GLenum zFail; GLenum zPass; };
    struct Blend_Custom   { GLenum srcFactor; GLenum destFactor; };

    //--------------------------------------------------------//

    /// Update Clip_Distance context state.
    void set_state(Clip_Distance state);

    /// Update Depth_Clamp context state.
    void set_state(Depth_Clamp state);

    /// Update Scissor_Test context state.
    void set_state(Scissor_Test state);

//    /// Update Alpha_Coverage context state.
//    void set_state(Alpha_Coverage state);

    /// Update Cull_Face context state.
    void set_state(Cull_Face state);

    /// Update Depth_Test context state.
    void set_state(Depth_Test state);

    /// Update Stencil_Test context state.
    void set_state(Stencil_Test state);

    /// Update Blend_Mode context state.
    void set_state(Blend_Mode state);

    /// Update Depth_Compare context state.
    void set_state(Depth_Compare state);

    //--------------------------------------------------------//

    /// Set the box used for scissor testing.
    void set_Scissor_Params(Scissor_Params args);

    /// Set mask parameters to use for stencil testing.
    void set_Stencil_Params(Stencil_Params args);

    /// Set custom Stencil_Test state parameters.
    void set_Stencil_Custom(Stencil_Custom args);

    /// Set custom Blend_Mode state parameters.
    void set_Blend_Custom(Blend_Custom args);

    //--------------------------------------------------------//

    /// Set the size of the drawing viewport.
    void set_ViewPort(Vec2U size);

    /// Get the size of the drawing viewport.
    Vec2U get_ViewPort() const { return crnt_ViewPort; }

    //--------------------------------------------------------//

    /// Bind a FrameBuffer for reading only.
    void bind_FrameBuffer_Read(const FrameBuffer& fbo);

    /// Bind a FrameBuffer for drawing only.
    void bind_FrameBuffer_Draw(const FrameBuffer& fbo);

    /// Bind a FrameBuffer for read and draw.
    void bind_FrameBuffer(const FrameBuffer& fbo);

    /// Bind the default FrameBuffer.
    void bind_FrameBuffer_default();

    //--------------------------------------------------------//

    /// Bind the specified Program.
    void bind_Program(const Program& program);

    /// Bind the the default Program.
    void bind_Program_default();

    //--------------------------------------------------------//

    /// Bind the specified VertexArray.
    void bind_VertexArray(const VertexArray& vao);

    /// Bind the dummy VertexArray.
    void bind_VertexArray_dummy();

    //--------------------------------------------------------//

    /// Bind a Texture to the specified slot.
    void bind_Texture(const Texture& tex, uint8_t slot);

    //--------------------------------------------------------//

    /// Bind a UniformBuffer to the specified index.
    void bind_UniformBuffer(const UniformBuffer& ubo, uint8_t index);

    /// Bind a portion of a UniformBuffer to the specified index.
    void bind_UniformBuffer(const UniformBuffer& ubo, uint8_t index, uint offset, uint size);

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
    Scissor_Test   crnt_Scissor_Test   = Scissor_Test   :: Disable;
//    Alpha_Coverage crnt_Alpha_Coverage = Alpha_Coverage :: Disable;
    Cull_Face      crnt_Cull_Face      = Cull_Face      :: Disable;
    Depth_Test     crnt_Depth_Test     = Depth_Test     :: Disable;
    Stencil_Test   crnt_Stencil_Test   = Stencil_Test   :: Disable;
    Blend_Mode     crnt_Blend_Mode     = Blend_Mode     :: Disable;
    Depth_Compare  crnt_Depth_Compare  = Depth_Compare  :: Less;

    Scissor_Params crnt_Scissor_Params;
    Stencil_Params crnt_Stencil_Params;
    Stencil_Custom crnt_Stencil_Custom;
    Blend_Custom   crnt_Blend_Custom;
    Vec2U          crnt_ViewPort;

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

constexpr bool operator==(const Context::Scissor_Params& a, const Context::Scissor_Params& b)
{ return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height; }

constexpr bool operator==(const Context::Stencil_Params& a, const Context::Stencil_Params& b)
{ return a.func == b.func && a.ref == b.ref && a.readMask == b.readMask && a.writeMask == b.writeMask; }

constexpr bool operator==(const Context::Stencil_Custom& a, const Context::Stencil_Custom& b)
{ return a.fail == b.fail && a.zFail == b.zFail && a.zPass == b.zPass; }

constexpr bool operator==(const Context::Blend_Custom& a, const Context::Blend_Custom& b)
{ return a.srcFactor == b.srcFactor && a.destFactor == b.destFactor; }

//============================================================================//

} // namespace sq
