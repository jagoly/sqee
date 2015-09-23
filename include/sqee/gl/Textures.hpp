#pragma once
#include <sqee/forward.hpp>

#include <sqee/misc/ResHolder.hpp>

namespace sq {

/// Abstract base class for OpenGL texture objects
class Texture : NonCopyable {
public:
    Texture();
    virtual ~Texture();

    /// A group of parameters and values to set at once
    using Preset = unordered_map<GLenum, GLenum>;

    /// Set a single texture paramater
    void set_param(GLenum _name, GLenum _value);

    /// Set a group of texture paramters
    void set_preset(Preset _preset);

    /// Generate mipmaps for the texture
    void generate_mipmaps();

    /// Activate a texture slot and bind the texture to it
    void bind(GLenum _slot);

    /// Bind the texture to the already active texture slot
    void bind();

    /// Return the size of the of texture
    uvec3 get_size();

    /// The OpenGL handle
    GLuint tex = 0u;

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
    GLenum target;
    GLenum format;
    GLenum iFormat;
    GLenum dataType;
    uint channels;
    bool mipmaps;
    uvec3 size;
};


/// A class for immutable GL_TEXTURE_2D textures
class Texture2D : public Texture {
public:
    /// Set various options and (re)create the OpenGL texture object
    void create(GLenum _format, GLenum _iFormat, uint _channels, bool _mipmaps);

    /// Transfer some memory into the texture
    void buffer_memory(const void* _data);

    /// Load a png or jpg image into the texture
    void buffer_file(const string& _path);

    /// Load an image, allocate storage and generate mipmaps
    void buffer_auto(const string& _path);

    /// Allocate immutable storage for the texture
    void allocate_storage(uvec2 _size);
};


/// A class for mutable GL_TEXTURE_2D textures
class TextureMut2D : public Texture {
public:
    /// Set various options and (re)create the OpenGL texture object
    void create(GLenum _format, GLenum _iFormat, uint _channels);

    /// Transfer some memory into the texture
    void buffer_memory(const void* _data);

    /// Resize the texture and allocate mutable storage
    void resize(uvec2 _size);
};


/// A class for immutable GL_TEXTURE_CUBE_MAP textures
class TextureCube : public Texture {
public:
    /// Set various options and (re)create the OpenGL texture object
    void create(GLenum _format, GLenum _iFormat, uint _channels, bool _mipmaps);

    /// Transfer some memory into one face of the texture
    void buffer_memory(const void* _data, uint _face);

    /// Load a png or jpg image into one face of the texture
    void buffer_file(const string& _path, uint _face);

    /// Load a directory of png or jpg images into the texture
    void buffer_full(const string& _path);

    /// Allocate immutable storage for the texture
    void allocate_storage(uint _size);
};


/// A class for mutable GL_TEXTURE_CUBE_MAP textures
class TextureMutCube : public Texture {
public:
    /// Set various options and (re)create the OpenGL texture object
    void create(GLenum _format, GLenum _iFormat, uint _channels);

    /// Transfer some memory into one face of the texture
    void buffer_memory(const void* _data, uint _face);

    /// Resize the texture and allocate mutable storage
    void resize(uint _size);
};


/// A class for immutable GL_TEXTURE_2D_ARRAY textures
class Texture2DArray : public Texture {
public:
    /// Set various options and (re)create the OpenGL texture object
    void create(GLenum _format, GLenum _iFormat, uint _channels, bool _mipmaps);

    /// Transfer some memory into one layer of the texture
    void buffer_memory(const void* _data, uint _index);

    /// Load a png or jpg image into one layer of the texture
    void buffer_file(const string& _path, uint _index);

    /// Allocate immutable storage for the texture
    void allocate_storage(uvec3 _size);
};


/// A class for mutable GL_TEXTURE_2D_ARRAY textures
class TextureMut2DArray : public Texture {
public:
    /// Set various options and (re)create the OpenGL texture object
    void create(GLenum _format, GLenum _iFormat, uint _channels);

    /// Transfer some memory into one layer of the texture
    void buffer_memory(const void* _data, uint _index);

    /// Resize the texture and allocate mutable storage
    void resize(uvec3 _size);
};


/// A class for immutable GL_TEXTURE_CUBE_MAP_ARRAY textures
class TextureCubeArray : public Texture {
public:
    /// Set various options and (re)create the OpenGL texture object
    void create(GLenum _format, GLenum _iFormat, uint _channels, bool _mipmaps);

    /// Transfer some memory into one layer-face of the texture
    void buffer_memory(const void* _data, uint _face, uint _index);

    /// Load a png or jpg image into one layer-face of the texture
    void buffer_file(const string& _path, uint _face, uint _index);

    /// Load a directory of six png or jpg images into the texture
    void buffer_full(const string& _path, uint _index);

    /// Allocate immutable storage for the texture
    void allocate_storage(uvec2 _size);
};


/// A class for mutable GL_TEXTURE_CUBE_MAP_ARRAY textures
class TextureMutCubeArray : public Texture {
public:
    /// Set various options and (re)create the OpenGL texture object
    void create(GLenum _format, GLenum _iFormat, uint _channels);

    /// Transfer some memory into one layer-face of the texture
    void buffer_memory(const void* _data, uint _face, uint _index);

    /// Resize the texture and allocate mutable storage
    void resize(uvec2 _size);
};


/// A class for immutable GL_TEXTURE_3D textures
class TextureVolume : public Texture {
public:
    /// Set various options and (re)create the OpenGL texture object
    void create(GLenum _format, GLenum _iFormat, uint _channels);

    /// Transfer some memory into the texture
    void buffer_memory(const void* _data);

    /// Allocate immutable storage for the texture
    void allocate_storage(uvec3 _size);
};


namespace res {
ResHolder<Texture2D>& tex2D();
ResHolder<TextureCube>& texCube();
}

}
