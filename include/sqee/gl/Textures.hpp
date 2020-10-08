// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>
#include <sqee/gl/Types.hpp>

#include <sqee/gl/SafeEnums.hpp>

namespace sq {

//============================================================================//

/// OpenGL Texture base class.
class SQEE_API Texture : private MoveOnly
{
public: //====================================================//

    Texture(Texture&& other) noexcept; ///< Move Constructor.

    Texture& operator=(Texture&& other) noexcept; ///< Move Assignment.

    ~Texture() noexcept; ///< Destructor.

    //--------------------------------------------------------//

    /// Automatically generate mipmaps.
    void generate_auto_mipmaps();

    //--------------------------------------------------------//

    /// Set the filtering mode to use.
    void set_filter_mode(bool filter, bool mipmaps);

    /// Enable shadow sampling.
    void set_shadow_mode(CompareFunc func);

    /// Set the swizzle modes to use (0, 1, R, G, B, A).
    void set_swizzle_mode(char r, char g, char b, char a);

    /// Set the wrap modes to use (C, R, M).
    void set_wrap_mode(char x, char y);

    //--------------------------------------------------------//

    /// Get the OpenGL object handle.
    GLuint get_handle() const { return mHandle; }

protected: //=================================================//

    Texture() = default; ///< Protected Constructor.

    GLuint mHandle = 0u;
};

//============================================================================//

/// GL_TEXTURE_2D Object.
class SQEE_API Texture2D final : public Texture
{
public: //====================================================//

    using Texture::Texture;

    /// Allocate storage for the texture.
    void allocate_storage(TexFormat format, Vec2U size, bool mipmaps);

    /// Copy some memory into a sub region of the texture.
    void load_memory(TexFormat format, uint level, Vec2U offset, Vec2U size, const void* data);

    /// Copy some memory into the base mipmap level.
    void load_memory(TexFormat format, Vec2U size, const void* data);

    /// Configure and load texture from a json and png file.
    void load_automatic(const String& path);

    /// Load an sq::Texture2D from sqee packages.
    static Texture2D make_from_package(const String& path);
};

//============================================================================//

/// GL_TEXTURE_2D_ARRAY Object.
class SQEE_API TextureArray final : public Texture
{
public: //====================================================//

    using Texture::Texture;

    /// Allocate storage for the texture.
    void allocate_storage(TexFormat format, Vec3U size, bool mipmaps);

    /// Copy some memory into a sub region of the texture.
    void load_memory(TexFormat format, uint level, Vec3U offset, Vec3U size, const void* data);

    /// Copy some memory into the base mipmap level of a layer.
    void load_memory(TexFormat format, uint layer, Vec2U size, const void* data);

    /// Configure and load texture from a directory.
    void load_automatic(const String& path);
};

//============================================================================//

/// GL_TEXTURE_CUBE_MAP Object.
class SQEE_API TextureCube final : public Texture
{
public: //====================================================//

    using Texture::Texture;

    /// Allocate storage for the texture.
    void allocate_storage(TexFormat format, uint size, bool mipmaps);

    /// Copy some memory into a sub region of the texture.
    void load_memory(TexFormat format, uint level, Vec3U offset, Vec3U size, const void* data);

    /// Copy some memory into the base mipmap level of a face.
    void load_memory(TexFormat format, uint face, uint size, const void* data);

    /// Configure and load texture from a directory.
    void load_automatic(const String& path);

};

//============================================================================//

/// GL_TEXTURE_2D_MULTISAMPLE Object.
class SQEE_API TextureMulti final : public Texture
{
public: //====================================================//

    using Texture::Texture;

    /// Allocate storage for the texture.
    void allocate_storage(TexFormat format, Vec2U size, uint samples);
};

//============================================================================//

} // namespace sq
