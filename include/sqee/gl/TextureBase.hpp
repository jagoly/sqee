// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/misc/Builtins.hpp>
#include <sqee/maths/Builtins.hpp>

namespace sq {

//============================================================================//

/// OpenGL Texture base class.
class SQEE_API Texture : private MoveOnly
{
public: //====================================================//

    /// Format for texture storage.
    enum class Format : int8_t
    {
        Undefined = -1,

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

    //--------------------------------------------------------//

    Texture(Texture&& other) noexcept; ///< Move Constructor.

    Texture& operator=(Texture&& other) noexcept; ///< Move Assignment.

    ~Texture() noexcept { delete_object(); } ///< Destructor.

    //--------------------------------------------------------//

    /// Delete the OpenGL object.
    void delete_object();

    /// Set the texture format.
    void set_format(Format format);

    /// Automatically generate mipmaps.
    void generate_auto_mipmaps();

    //--------------------------------------------------------//

    /// Get the target of the texture.
    GLenum get_target() const { return mTarget; }

    /// Get the format of the texture.
    Format get_format() const { return mFormat; }

    /// Get the OpenGL object handle.
    GLuint get_handle() const { return mHandle; }

    /// Get the size of the texture.
    Vec3U get_size() const { return mSize; }

    //--------------------------------------------------------//

    /// Enable or disable filtered sampling.
    void set_filter_mode(bool enable);

    /// Enable or disable mipmap support.
    void set_mipmaps_mode(bool enable);

    /// Enable or disable shadow sampling.
    void set_shadow_mode(bool enable);

    //--------------------------------------------------------//

    /// Set the wrap modes to use (C, R, M).
    void set_wrap_mode(char x, char y);

    /// Set the swizzle modes to use (0, 1, R, G, B, A).
    void set_swizzle_mode(char r, char g, char b, char a);

protected: //=================================================//

    /// Protected Constructor.
    Texture(GLenum target, Format format);

private: //===================================================//

    friend class Texture2D;
    friend class TextureCube;
    friend class TextureArray2D;
    friend class TextureArrayCube;
    friend class TextureVolume;
    friend class TextureMulti;

    //--------------------------------------------------------//

    class Context& mContext;

    const GLenum mTarget;

    Format mFormat;

    GLuint mHandle = 0u;

    //--------------------------------------------------------//

    bool mFilter = false;
    bool mMipmaps = false;
    bool mShadow = false;

    //--------------------------------------------------------//

    Array<char, 2> mWrap {{ 'C', 'C' }};
    Array<char, 4> mSwizzle {{ 'R', 'G', 'B', 'A' }};

    //--------------------------------------------------------//

    Vec3U mSize = { 0u, 0u, 0u };

    //--------------------------------------------------------//

    void impl_create_object();
    void impl_update_paramaters();

    //--------------------------------------------------------//

    static Format impl_string_to_format(const String& arg);
};

//============================================================================//

} // namespace sq
