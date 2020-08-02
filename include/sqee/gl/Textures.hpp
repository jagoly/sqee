// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

namespace sq {

//====== Forward Declarations ================================================//

class Context;

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

    Texture(GLenum target, Format format);

    void impl_create_object();

    void impl_update_paramaters();

    //--------------------------------------------------------//

    Context& mContext;

    const GLenum mTarget;

    Format mFormat;

    GLuint mHandle = 0u;

    //--------------------------------------------------------//

    bool mFilter = false;
    bool mMipmaps = false;
    bool mShadow = false;

    std::array<char, 2> mWrap {{ 'C', 'C' }};
    std::array<char, 4> mSwizzle {{ 'R', 'G', 'B', 'A' }};

    Vec3U mSize = { 0u, 0u, 0u };
};

//============================================================================//

/// GL_TEXTURE_2D Object.
class SQEE_API Texture2D final : public Texture
{
public: //====================================================//

    /// Constructor.
    Texture2D(Format format = Format::Undefined);

    /// Allocate storage for the texture.
    void allocate_storage(Vec2U size);

    //--------------------------------------------------------//

    /// Transfer some memory into the texture.
    void load_memory(const void* data);

    //--------------------------------------------------------//

    /// Load a png or jpg image into the texture.
    void load_file(StringView path);

    /// Configure and load texture with a meta.json file.
    void load_automatic(StringView path);

    //--------------------------------------------------------//

    /// Load an sq::Texture2D from sqee packages.
    static Texture2D make_from_package(const String& path);
};

//============================================================================//

/// GL_TEXTURE_CUBE_MAP Object.
class SQEE_API TextureCube final : public Texture
{
public: //====================================================//

    /// Constructor.
    TextureCube(Format format = Format::Undefined);

    /// Allocate storage for the texture.
    void allocate_storage(uint size);

    //--------------------------------------------------------//

    /// Transfer some memory into the texture.
    void load_memory(const void* data);

    /// Transfer some memory into one face of the texture.
    void load_memory(const void* data, uint face);

    //--------------------------------------------------------//

    /// Load a png or jpg image into one face of the texture.
    void load_file(StringView path, uint face);

    /// Configure and load texture with a meta.json file.
    void load_automatic(StringView path);
};

//============================================================================//

/// GL_TEXTURE_2D_ARRAY Object.
class SQEE_API TextureArray2D final : public Texture
{
public: //====================================================//

    /// Constructor.
    TextureArray2D(Format format = Format::Undefined);

    /// Allocate storage for the texture.
    void allocate_storage(Vec3U size);

    //--------------------------------------------------------//

    /// Transfer some memory into the texture.
    void load_memory(const void* data);

    /// Transfer some memory into one layer of the texture.
    void load_memory(const void* data, uint layer);

    //--------------------------------------------------------//

    /// Load a png or jpg image into one layer of the texture.
    void load_file(StringView path, uint layer);

    /// Configure and load texture with a meta.json file.
    void load_automatic(StringView path);
};

//============================================================================//

///// GL_TEXTURE_CUBE_MAP_ARRAY Object.
//class SQEE_API TextureArrayCube final : public Texture
//{
//public: //====================================================//

//    /// Constructor.
//    TextureArrayCube(Format format = Format::Undefined);

//    /// Allocate storage for the texture.
//    void allocate_storage(Vec2U size);

//    //--------------------------------------------------------//

//    /// Transfer some memory into one layer-face of the texture.
//    void buffer_memory(const void* data, uint face, uint layer, GLenum type);

//    /// Load a png or jpg image into one layer-face of the texture.
//    void buffer_file(const string& path, uint face, uint layer);

//    /// Load a directory of six png or jpg images into the texture.
//    void buffer_full(const string& path, uint layer);
//};

//============================================================================//

/// GL_TEXTURE_3D Object.
class SQEE_API TextureVolume final : public Texture
{
public: //====================================================//

    /// Constructor.
    TextureVolume(Format format = Format::Undefined);

    /// Allocate storage for the texture.
    void allocate_storage(Vec3U size);

    /// Transfer some memory into a region of the texture.
    void load_memory(const void* data, Vec3U offset, Vec3U size);
};

//============================================================================//

/// GL_TEXTURE_2D_MULTISAMPLE Object.
class SQEE_API TextureMulti final : public Texture
{
public: //====================================================//

    /// Constructor.
    TextureMulti(Format format = Format::Undefined);

    /// Allocate storage for the texture.
    void allocate_storage(Vec3U size);
};

//============================================================================//

} // namespace sq
