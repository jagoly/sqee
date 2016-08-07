#pragma once

#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sq {

/// Abstract base class for OpenGL texture objects
class Texture : NonCopyable {
public:
    virtual ~Texture();

    /// A group of parameters and values to set at once
    using Preset = vector<pair<GLenum, GLenum>>;

    /// Manually delete the GL object
    void delete_object();

    /// Generate mipmaps for the texture
    void generate_mipmaps() const;

    /// Return the size of the texture
    Vec3U get_size() const { return size; }

    /// Return the texture target
    GLenum get_target() const { return target; }

    /// Return the format of texture
    GLenum get_format() const { return format; }

    /// Return the internal format of texture
    GLenum get_internal_format() const { return iFormat; }

    /// Get the OpenGL handle
    const GLuint& get_handle() const { return tex; }

    static Preset NearestClamp();
    static Preset NearestRepeat();
    static Preset NearestMirror();
    static Preset LinearClamp();
    static Preset LinearRepeat();
    static Preset LinearMirror();
    static Preset MipmapClamp();
    static Preset MipmapRepeat();
    static Preset MipmapMirror();
    static Preset ShadowMap();

protected:
    Texture(GLenum _target, GLenum _format, GLenum _iFormat, Preset _preset) :
        target(_target), format(_format), iFormat(_iFormat), preset(_preset) {};
    const GLenum target, format, iFormat; const Preset preset; Vec3U size;

    /// The OpenGL handle
    GLuint tex = 0u;

    void create_and_setup_texture();
};


/// A class for immutable GL_TEXTURE_2D textures
class Texture2D final : public Texture {
public:
    /// Constructor
    Texture2D(GLenum _format, GLenum _iFormat, Preset _preset);

    /// Transfer some memory into the texture
    void buffer_memory(const void* _data, GLenum _type);

    /// Load a png or jpg image into the texture
    void buffer_file(const string& _path);

    /// Load an image, allocate storage and generate mipmaps
    void buffer_auto(const string& _path, bool _mipmaps);

    /// Allocate immutable storage for the texture
    void allocate_storage(Vec2U _size, bool _mipmaps);
};

/// Create, setup and load a texture automatically
unique_ptr<Texture2D> load_Texture2D(const string& _path);


/// A class for immutable GL_TEXTURE_CUBE_MAP textures
class TextureCube final : public Texture {
public:
    /// Constructor
    TextureCube(GLenum _format, GLenum _iFormat, Preset _preset);

    /// Transfer some memory into one face of the texture
    void buffer_memory(const void* _data, uint _face, GLenum _type);

    /// Load a png or jpg image into one face of the texture
    void buffer_file(const string& _path, uint _face);

    /// Load a directory of png or jpg images into the texture
    void buffer_full(const string& _path);

    /// Allocate immutable storage for the texture
    void allocate_storage(uint _size, bool _mipmaps);
};


/// A class for immutable GL_TEXTURE_2D_ARRAY textures
class Texture2DArray final : public Texture {
public:
    /// Constructor
    Texture2DArray(GLenum _format, GLenum _iFormat, Preset _preset);

    /// Transfer some memory into one layer of the texture
    void buffer_memory(const void* _data, uint _index, GLenum _type);

    /// Load a png or jpg image into one layer of the texture
    void buffer_file(const string& _path, uint _index);

    /// Allocate immutable storage for the texture
    void allocate_storage(Vec3U _size, bool _mipmaps);
};


/// A class for immutable GL_TEXTURE_CUBE_MAP_ARRAY textures
class TextureCubeArray final : public Texture {
public:
    /// Constructor
    TextureCubeArray(GLenum _format, GLenum _iFormat, Preset _preset);

    /// Transfer some memory into one layer-face of the texture
    void buffer_memory(const void* _data, uint _face, uint _index, GLenum _type);

    /// Load a png or jpg image into one layer-face of the texture
    void buffer_file(const string& _path, uint _face, uint _index);

    /// Load a directory of six png or jpg images into the texture
    void buffer_full(const string& _path, uint _index);

    /// Allocate immutable storage for the texture
    void allocate_storage(Vec2U _size, bool _mipmaps);
};


/// A class for immutable GL_TEXTURE_3D textures
class TextureVolume final : public Texture {
public:
    /// Constructor
    TextureVolume(GLenum _format, GLenum _iFormat, Preset _preset);

    /// Transfer some memory into the texture
    void buffer_memory(const void* _data, GLenum _type);

    /// Allocate immutable storage for the texture
    void allocate_storage(Vec3U _size);
};

}
