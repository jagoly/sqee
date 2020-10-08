// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/gl/Constants.hpp>

namespace sq {

//============================================================================//

enum class BlitMask : GLbitfield
{
    Depth              = gl::DEPTH_BUFFER_BIT,
    Stencil            = gl::STENCIL_BUFFER_BIT,
    Colour             = gl::COLOR_BUFFER_BIT,
    DepthStencil       = Depth | Stencil,
    DepthColour        = Depth | Colour,
    StencilColour      = Stencil | Colour,
    DepthStencilColour = Depth | Stencil | Colour,
};

enum class FboAttach : GLenum
{
    DepthStencil = gl::DEPTH_STENCIL_ATTACHMENT,
    Colour0      = gl::COLOR_ATTACHMENT0,
    Colour1      = gl::COLOR_ATTACHMENT1,
    Colour2      = gl::COLOR_ATTACHMENT2,
    Colour3      = gl::COLOR_ATTACHMENT3,
    Colour4      = gl::COLOR_ATTACHMENT4,
    Colour5      = gl::COLOR_ATTACHMENT5,
    Colour6      = gl::COLOR_ATTACHMENT6,
    Colour7      = gl::COLOR_ATTACHMENT7,
    Depth        = gl::DEPTH_ATTACHMENT,
    Stencil      = gl::STENCIL_ATTACHMENT
};

//============================================================================//

enum class TexTarget : GLenum
{
    //Undefined = 0xFFFFFFFF,

    Tex2D    = gl::TEXTURE_2D,
    TexArray = gl::TEXTURE_2D_ARRAY,
    TexCube  = gl::TEXTURE_CUBE_MAP,
    TexMulti = gl::TEXTURE_2D_MULTISAMPLE
};

enum class TexFormat : uint8_t
{
    // unsigned normalised /////
    R8_UN,  RG8_UN,  RGB8_UN,  RGBA8_UN,
    R16_UN, RG16_UN, RGB16_UN, RGBA16_UN,

    // signed normalised /////
    R8_SN,  RG8_SN,  RGB8_SN,  RGBA8_SN,
    R16_SN, RG16_SN, RGB16_SN, RGBA16_SN,

    // unsigned integral /////
    R8_UI,  RG8_UI,  RGB8_UI,  RGBA8_UI,
    R16_UI, RG16_UI, RGB16_UI, RGBA16_UI,
    R32_UI, RG32_UI, RGB32_UI, RGBA32_UI,

    // signed integral /////
    R8_SI,  RG8_SI,  RGB8_SI,  RGBA8_SI,
    R16_SI, RG16_SI, RGB16_SI, RGBA16_SI,
    R32_SI, RG32_SI, RGB32_SI, RGBA32_SI,

    // floating point /////
    R16_FP, RG16_FP, RGB16_FP, RGBA16_FP,
    R32_FP, RG32_FP, RGB32_FP, RGBA32_FP,

    // depth, stencil /////
    DEPTH16, STENCIL8, DEP24S8
};

/*enum class TexFormat : GLenum
{
    Undefined = GLenum(-1),

    R8_UN    = gl::R8,
    RG8_UN   = gl::RG8,
    RGB8_UN  = gl::RGB8,
    RGBA8_UN = gl::RGBA8,

    R16_UN    = gl::R16,
    RG16_UN   = gl::RG16,
    RGB16_UN  = gl::RGB16,
    RGBA16_UN = gl::RGBA16,

    R8_SN    = gl::R8_SNORM,
    RG8_SN   = gl::RG8_SNORM,
    RGB8_SN  = gl::RGB8_SNORM,
    RGBA8_SN = gl::RGBA8_SNORM,

    R16_SN    = gl::R16_SNORM,
    RG16_SN   = gl::RG16_SNORM,
    RGB16_SN  = gl::RGB16_SNORM,
    RGBA16_SN = gl::RGBA16_SNORM,

    R16_FP    = gl::R16F,
    RG16_FP   = gl::RG16F,
    RGB16_FP  = gl::RGB16F,
    RGBA16_FP = gl::RGBA16F,

    R32_FP    = gl::R32F,
    RG32_FP   = gl::RG32F,
    RGB32_FP  = gl::RGB32F,
    RGBA32_FP = gl::RGBA32F,

    DEPTH16  = gl::DEPTH_COMPONENT16,
    STENCIL8 = gl::STENCIL_INDEX8,
    DEP24S8  = gl::DEPTH24_STENCIL8

//    R8_UI,  RG8_UI,  RGB8_UI,  RGBA8_UI,
//    R16_UI, RG16_UI, RGB16_UI, RGBA16_UI,
//    R32_UI, RG32_UI, RGB32_UI, RGBA32_UI,
//    R8_SI,  RG8_SI,  RGB8_SI,  RGBA8_SI,
//    R16_SI, RG16_SI, RGB16_SI, RGBA16_SI,
//    R32_SI, RG32_SI, RGB32_SI, RGBA32_SI,
};*/

//============================================================================//

enum class ShaderStage : GLenum
{
    Vertex   = gl::VERTEX_SHADER,
    Geometry = gl::GEOMETRY_SHADER,
    Fragment = gl::FRAGMENT_SHADER
};

enum class CompareFunc : GLenum
{
    Never        = gl::NEVER,
    Less         = gl::LESS,
    LessEqual    = gl::LEQUAL,
    Greater      = gl::GREATER,
    GreaterEqual = gl::GEQUAL,
    Equal        = gl::EQUAL,
    NotEqual     = gl::NOTEQUAL,
    Always       = gl::ALWAYS
};

enum class StencilOp : GLenum
{
    Keep          = gl::KEEP,
    Zero          = gl::ZERO,
    Replace       = gl::REPLACE,
    Increment     = gl::INCR,
    IncrementWrap = gl::INCR_WRAP,
    Decrement     = gl::DECR,
    DecrementWrap = gl::DECR_WRAP,
    Invert        = gl::INVERT
};

enum class BlendFactor : GLenum
{
    Zero              = gl::ZERO,
    One               = gl::ONE,
    SrcColour         = gl::SRC_COLOR,
    SrcColourInverse  = gl::ONE_MINUS_SRC_COLOR,
    SrcAlpha          = gl::SRC_ALPHA,
    SrcAlphaInverse   = gl::ONE_MINUS_SRC_ALPHA,
    DestColour        = gl::DST_COLOR,
    DestColourInverse = gl::ONE_MINUS_DST_COLOR,
    DestAlpha         = gl::DST_ALPHA,
    DestAlphaInverse  = gl::ONE_MINUS_DST_ALPHA,

    //SrcAlphaSaturate      = gl::SRC_ALPHA_SATURATE,
    //ConstantColour        = gl::CONSTANT_COLOR,
    //ConstantColourInverse = gl::ONE_MINUS_CONSTANT_COLOR,
    //ConstantAlpha         = gl::CONSTANT_ALPHA,
    //ConstantAlphaIverse   = gl::ONE_MINUS_CONSTANT_ALPHA,
    //Src1Colour            = gl::SRC1_COLOR,
    //Src1ColourInverse     = gl::ONE_MINUS_SRC1_COLOR,
    //Src1Alpha             = gl::SRC1_ALPHA,
    //Src1AlphaInverse      = gl::ONE_MINUS_SRC1_ALPHA
};

enum class BufTarget : GLenum
{
    Uniform       = gl::UNIFORM_BUFFER,
    ShaderStorage = gl::SHADER_STORAGE_BUFFER
};

enum class FboTarget : GLenum
{
    Read = gl::READ_FRAMEBUFFER,
    Draw = gl::DRAW_FRAMEBUFFER,
    Both = gl::FRAMEBUFFER
};

//============================================================================//

enum class DrawPrimitive : GLenum
{
    Points        = gl::POINTS,
    Lines         = gl::LINES,
    LineLoop      = gl::LINE_LOOP,
    LineStrip     = gl::LINE_STRIP,
    Triangles     = gl::TRIANGLES,
    TriangleStrip = gl::TRIANGLE_STRIP,
    TriangleFan   = gl::TRIANGLE_FAN,

    //LinesAdjacency         = gl::LINES_ADJACENCY,
    //LineStripAdjacency     = gl::LINE_STRIP_ADJACENCY,
    //TrianglesAdjacency     = gl::TRIANGLES_ADJACENCY,
    //TriangleStripAdjacency = gl::TRIANGLE_STRIP_ADJACENCY,
    //Patches                = gl::PATCHES
};

//============================================================================//

enum class AlphaCoverage : bool
{
    Disable,
    Enable
};

enum class ClipDistance : bool
{
    Disable,
    Enable
};

enum class DepthClamp : bool
{
    Disable,
    Enable
};

enum class ScissorTest : bool
{
    Disable,
    Enable
};

//============================================================================//

enum class CullFace : uint8_t
{
    Disable,
    Back,
    Front
};

enum class DepthTest : uint8_t
{
    Disable,
    Keep,
    Replace
};

enum class StencilTest : uint8_t
{
    Disable,
    Keep,
    Replace,
    Custom
};

enum class BlendMode : uint8_t
{
    Disable,
    Accumulate,
    Alpha,
    PremAlpha,
    Custom
};

//============================================================================//

} // namespace sq::gl
