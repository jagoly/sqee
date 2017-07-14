#pragma once

#include <sqee/gl/TextureBase.hpp>

namespace sq {

//============================================================================//

/// GL_TEXTURE_2D Object.
class Texture2D final : public Texture
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
    void load_file(const string& path);

    /// Load an image, allocate storage and generate mipmaps
    //void load_auto(const string& path, bool mipmaps);

    //--------------------------------------------------------//

    /// Load an sq::Texture2D from sqee packages.
    static Texture2D make_from_package(const string& path);
};

//============================================================================//

/// GL_TEXTURE_CUBE_MAP Object.
class TextureCube final : public Texture
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
    void load_file(const string& path, uint face);

    /// Load a directory of png or jpg images into the texture.
    void load_directory(const string& path);
};

//============================================================================//

/// GL_TEXTURE_2D_ARRAY Object.
class TextureArray2D final : public Texture
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
    void load_file(const string& path, uint layer);
};

//============================================================================//

///// GL_TEXTURE_CUBE_MAP_ARRAY Object.
//class TextureArrayCube final : public Texture
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
class TextureVolume final : public Texture
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
class TextureMulti final : public Texture
{
public: //====================================================//

    /// Constructor.
    TextureMulti(Format format = Format::Undefined);

    /// Allocate storage for the texture.
    void allocate_storage(Vec3U size);
};

//============================================================================//

} // namespace sq
