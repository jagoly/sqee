#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/redist/stb_image.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/misc/Files.hpp>

using namespace sq;

// Helpers /////

struct LoadedImage {
    LoadedImage(uint _w, uint _h, uchar* _d) : width(_w), height(_h), data(_d) {}
    int width = 0; int height = 0; uchar* data = NULL;
    ~LoadedImage() { if (data != NULL) free(data); }
};

LoadedImage load_image(const string _path, uint _channels) {
    int width = 0; int height = 0; uchar* data = NULL;
    data = stbi_load(_path.c_str(), &width, &height, 0, _channels);
    uchar* top; uchar* btm; uchar temp; int wBytes = width * _channels;

    int halfH = height / 2;
    for (int y = 0; y < halfH; ++y) {
        top = data + y * wBytes;
        btm = data + (height - y - 1) * wBytes;
        for (int x = 0; x < wBytes; ++x) {
            temp = *top; *top = *btm;
            *btm = temp; ++top; ++btm;
        }
    }

    return LoadedImage(width, height, data);
}


// sq::Texture /////

Texture::Texture() {
    gl::GenTextures(1, &tex);
}

Texture::~Texture() {
    gl::DeleteTextures(1, &tex);
}

void Texture::set_param(GLenum _name, GLenum _value) {
    gl::BindTexture(target, tex);
    gl::TexParameteri(target, _name, _value);
}

void Texture::set_preset(Preset _preset) {
    gl::BindTexture(target, tex);
    for (const auto& val : _preset)
        gl::TexParameteri(target, val.first, val.second);
}

void Texture::generate_mipmaps() {
    gl::BindTexture(target, tex);
    gl::GenerateMipmap(target);
}

void Texture::bind(GLenum _slot) {
    gl::ActiveTexture(_slot);
    gl::BindTexture(target, tex);
}

void Texture::bind() {
    gl::BindTexture(target, tex);
}

uvec3 Texture::get_size() {
    return size;
}


// sq::Texture2D /////

void Texture2D::create(GLenum _format, GLenum _iFormat, uint _channels, bool _mipmaps) {
    format = _format; iFormat = _iFormat; channels = _channels; mipmaps = _mipmaps;
    dataType = format == gl::DEPTH_STENCIL ? gl::UNSIGNED_INT_24_8 : gl::UNSIGNED_BYTE;
    target = gl::TEXTURE_2D;
}

void Texture2D::buffer_memory(const void* _data) {
    gl::BindTexture(target, tex);
    gl::TexSubImage2D(target, 0, 0, 0, size.x, size.y, format, dataType, _data);
}

void Texture2D::buffer_file(const string& _path) {
    string path = res::path() + "textures/" + _path;
    if (check_file_exists(path+".png")) path += ".png";
    else if (check_file_exists(path+".jpg")) path += ".jpg";

    LoadedImage img = load_image(path, channels);
    if (!img.data) log_error("Failed to load texture from %s", path);
    else { buffer_memory(img.data); }
}

void Texture2D::buffer_auto(const string& _path) {
    string path = res::path() + "textures/" + _path;
    if (check_file_exists(path+".png")) path += ".png";
    else if (check_file_exists(path+".jpg")) path += ".jpg";

    LoadedImage img = load_image(path, channels);
    if (!img.data) log_error("Failed to load texture from %s", path);
    else { allocate_storage({img.width, img.height}); buffer_memory(img.data);
           if (mipmaps == true) generate_mipmaps(); }
}

void Texture2D::allocate_storage(uvec2 _size) {
    gl::BindTexture(target, tex); size = uvec3(_size.x, _size.y, 0u);
    uint maxLevel = mipmaps ? std::floor(std::log2(std::max(size.x, size.y))) + 1u : 1u;
    gl::TexStorage2D(target, maxLevel, iFormat, size.x, size.y);
}


// sq::TextureMut2D /////

void TextureMut2D::create(GLenum _format, GLenum _iFormat, uint _channels) {
    format = _format; iFormat = _iFormat; channels = _channels;
    dataType = format == gl::DEPTH_STENCIL ? gl::UNSIGNED_INT_24_8 : gl::UNSIGNED_BYTE;
    target = gl::TEXTURE_2D;
}

void TextureMut2D::buffer_memory(const void* _data) {
    gl::BindTexture(target, tex);
    gl::TexSubImage2D(target, 0, 0, 0, size.x, size.y, format, dataType, _data);
}

void TextureMut2D::resize(uvec2 _size) {
    gl::BindTexture(target, tex); size = uvec3(_size.x, _size.y, 0u);
    gl::TexImage2D(target, 0, iFormat, size.x, size.y, 0, format, dataType, nullptr);
}


// sq::TextureCube /////

void TextureCube::create(GLenum _format, GLenum _iFormat, uint _channels, bool _mipmaps) {
    format = _format; iFormat = _iFormat; channels = _channels; mipmaps = _mipmaps;
    dataType = format == gl::DEPTH_STENCIL ? gl::UNSIGNED_INT_24_8 : gl::UNSIGNED_BYTE;
    target = gl::TEXTURE_CUBE_MAP;
}

void TextureCube::buffer_memory(const void* _data, uint _face) {
    gl::BindTexture(target, tex);
    uint faceYZ[6] = {0u, 1u, 2u, 3u, 4u, 5u};
    uint face = gl::TEXTURE_CUBE_MAP_POSITIVE_X + faceYZ[_face];
    gl::TexSubImage2D(face, 0, 0, 0, size.x, size.x, format, dataType, _data);
}

void TextureCube::buffer_file(const string& _path, uint _face) {
    string path = res::path() + "textures/" + _path;
    if (check_file_exists(path+".png")) path += ".png";
    else if (check_file_exists(path+".jpg")) path += ".jpg";

    LoadedImage img = load_image(path, channels);
    if (!img.data) log_error("Failed to load texture from %s", path);
    else { buffer_memory(img.data, _face); }
}

void TextureCube::buffer_full(const string& _path) {
    buffer_file(_path+"/x+", 0);
    buffer_file(_path+"/x-", 1);
    buffer_file(_path+"/y+", 2);
    buffer_file(_path+"/y-", 3);
    buffer_file(_path+"/z+", 4);
    buffer_file(_path+"/z-", 5);
}

void TextureCube::allocate_storage(uint _size) {
    gl::BindTexture(target, tex); size = uvec3(_size, _size, 0u);
    uint maxLevel = mipmaps ? std::floor(std::log2(size.x)) + 1u : 1u;
    gl::TexStorage2D(target, maxLevel, iFormat, size.x, size.x);
}


// sq::TextureMutCube /////

void TextureMutCube::create(GLenum _format, GLenum _iFormat, uint _channels) {
    format = _format; iFormat = _iFormat; channels = _channels;
    dataType = format == gl::DEPTH_STENCIL ? gl::UNSIGNED_INT_24_8 : gl::UNSIGNED_BYTE;
    target = gl::TEXTURE_CUBE_MAP;
}

void TextureMutCube::buffer_memory(const void* _data, uint _face) {
    gl::BindTexture(target, tex);
    uint faceYZ[6] = {0u, 1u, 2u, 3u, 4u, 5u};
    uint face = gl::TEXTURE_CUBE_MAP_POSITIVE_X + faceYZ[_face];
    gl::TexSubImage2D(face, 0, 0, 0, size.x, size.x, format, dataType, _data);
}

void TextureMutCube::resize(uint _size) {
    gl::BindTexture(target, tex); size = uvec3(_size, _size, 0u);
    gl::TexImage2D(gl::TEXTURE_CUBE_MAP_POSITIVE_X, 0, iFormat, _size, _size, 0, format, dataType, nullptr);
    gl::TexImage2D(gl::TEXTURE_CUBE_MAP_NEGATIVE_X, 0, iFormat, _size, _size, 0, format, dataType, nullptr);
    gl::TexImage2D(gl::TEXTURE_CUBE_MAP_POSITIVE_Z, 0, iFormat, _size, _size, 0, format, dataType, nullptr);
    gl::TexImage2D(gl::TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, iFormat, _size, _size, 0, format, dataType, nullptr);
    gl::TexImage2D(gl::TEXTURE_CUBE_MAP_POSITIVE_Y, 0, iFormat, _size, _size, 0, format, dataType, nullptr);
    gl::TexImage2D(gl::TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, iFormat, _size, _size, 0, format, dataType, nullptr);
}


// sq::Texture2DArray /////

void Texture2DArray::create(GLenum _format, GLenum _iFormat, uint _channels, bool _mipmaps) {
    format = _format; iFormat = _iFormat; channels = _channels; mipmaps = _mipmaps;
    dataType = format == gl::DEPTH_STENCIL ? gl::UNSIGNED_INT_24_8 : gl::UNSIGNED_BYTE;
    target = gl::TEXTURE_2D_ARRAY;
}

void Texture2DArray::buffer_memory(const void* _data, uint _index) {
    gl::BindTexture(target, tex);
    gl::TexSubImage3D(target, 0, 0, 0, _index, size.x, size.y, 1, format, dataType, _data);
}

void Texture2DArray::buffer_file(const string& _path, uint _index) {
    string path = res::path() + "textures/" + _path;
    if (check_file_exists(path+".png")) path += ".png";
    else if (check_file_exists(path+".jpg")) path += ".jpg";

    LoadedImage img = load_image(path, channels);
    if (!img.data) log_error("Failed to load texture from %s", path);
    else { buffer_memory(img.data, _index); }
}

void Texture2DArray::allocate_storage(uvec3 _size) {
    gl::BindTexture(target, tex); size = uvec3(_size.x, _size.y, _size.z);
    uint maxLevel = mipmaps ? std::floor(std::log2(std::max(size.x, size.y))) + 1u : 1u;
    gl::TexStorage3D(target, maxLevel, iFormat, size.x, size.y, size.z);
}


// sq::TextureMut2DArray /////

void TextureMut2DArray::create(GLenum _format, GLenum _iFormat, uint _channels) {
    format = _format; iFormat = _iFormat; channels = _channels;
    dataType = format == gl::DEPTH_STENCIL ? gl::UNSIGNED_INT_24_8 : gl::UNSIGNED_BYTE;
    target = gl::TEXTURE_2D_ARRAY;
}

void TextureMut2DArray::buffer_memory(const void* _data, uint _index) {
    gl::BindTexture(target, tex);
    gl::TexSubImage3D(target, 0, 0, 0, _index, size.x, size.y, 1, format, dataType, _data);
}

void TextureMut2DArray::resize(uvec3 _size) {
    gl::BindTexture(target, tex); size = uvec3(_size.x, _size.y, _size.z);
    gl::TexImage3D(target, 0, iFormat, size.x, size.y, size.z, 0, format, dataType, nullptr);
}


// sq::TextureCubeArray /////

void TextureCubeArray::create(GLenum _format, GLenum _iFormat, uint _channels, bool _mipmaps) {
    format = _format; iFormat = _iFormat; channels = _channels; mipmaps = _mipmaps;
    dataType = format == gl::DEPTH_STENCIL ? gl::UNSIGNED_INT_24_8 : gl::UNSIGNED_BYTE;
    target = gl::TEXTURE_CUBE_MAP_ARRAY;
}

void TextureCubeArray::buffer_memory(const void* _data, uint _face, uint _index) {
    uint faceYZ[6] = {0u, 1u, 2u, 3u, 4u, 5u};
    gl::BindTexture(target, tex); int index = _index * 6 + faceYZ[_face];
    gl::TexSubImage3D(target, 0, 0, 0, index, size.x, size.y, 1, format, dataType, _data);
}

void TextureCubeArray::buffer_file(const string& _path, uint _face, uint _index) {
    string path = res::path() + "textures/" + _path;
    if (check_file_exists(path+".png")) path += ".png";
    else if (check_file_exists(path+".jpg")) path += ".jpg";

    LoadedImage img = load_image(path, channels);
    if (!img.data) log_error("Failed to load texture from %s", path);
    else { buffer_memory(img.data, _face, _index); }
}

void TextureCubeArray::buffer_full(const string& _path, uint _index) {
    buffer_file(_path+"/x+", 0, _index);
    buffer_file(_path+"/x-", 1, _index);
    buffer_file(_path+"/y+", 2, _index);
    buffer_file(_path+"/y-", 3, _index);
    buffer_file(_path+"/z+", 4, _index);
    buffer_file(_path+"/z-", 5, _index);
}

void TextureCubeArray::allocate_storage(uvec2 _size) {
    gl::BindTexture(target, tex); size = uvec3(_size.x, _size.x, _size.y);
    uint maxLevel = mipmaps ? std::floor(std::log2(size.x)) + 1u : 1u;
    gl::TexStorage3D(target, maxLevel, iFormat, size.x, size.y, size.z * 6);
}


// sq::TextureMutCubeArray /////

void TextureMutCubeArray::create(GLenum _format, GLenum _iFormat, uint _channels) {
    format = _format; iFormat = _iFormat; channels = _channels;
    dataType = format == gl::DEPTH_STENCIL ? gl::UNSIGNED_INT_24_8 : gl::UNSIGNED_BYTE;
    target = gl::TEXTURE_CUBE_MAP_ARRAY;
}

void TextureMutCubeArray::buffer_memory(const void* _data, uint _face, uint _index) {
    uint faceYZ[6] = {0u, 1u, 4u, 5u, 2u, 3u};
    gl::BindTexture(target, tex); int index = _index * 6 + faceYZ[_face];
    gl::TexSubImage3D(target, 0, 0, 0, index, size.x, size.y, 1, format, dataType, _data);
}

void TextureMutCubeArray::resize(uvec2 _size) {
    gl::BindTexture(target, tex); size = uvec3(_size.x, _size.x, _size.y);
    gl::TexImage3D(target, 0, iFormat, size.x, size.x, size.z * 6, 0, format, dataType, nullptr);
}


// sq::TextureVolume /////

void TextureVolume::create(GLenum _format, GLenum _iFormat, uint _channels) {
    format = _format; iFormat = _iFormat; channels = _channels;
    dataType = gl::UNSIGNED_BYTE; target = gl::TEXTURE_3D;
}

void TextureVolume::buffer_memory(const void* _data) {
    gl::BindTexture(target, tex);
    struct MD { float x, y, z, index; };
    for (uint i = 0u; i < 12u*5u*5u; ++i) {
        std::cout << i << " " << ((MD*)_data)[i].z << std::endl;
    }
    gl::TexSubImage3D(target, 0, 0, 0, 0, size.x, size.y, size.z, format, gl::FLOAT, _data);
    //gl::TexImage3D(target, 0u, iFormat, size.x, size.y, size.z, 0, format, gl::FLOAT, _data);
}

void TextureVolume::allocate_storage(uvec3 _size) {
    gl::BindTexture(target, tex); size = _size;
    gl::TexStorage3D(target, 1u, iFormat, size.x, size.y, size.z);
}


// Texture Presets ////

Texture::Preset Texture::NearestClamp() {
    return Preset {
        {gl::TEXTURE_MIN_FILTER, gl::NEAREST},
        {gl::TEXTURE_MAG_FILTER, gl::NEAREST},
        {gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE},
        {gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE},
        {gl::TEXTURE_WRAP_R, gl::CLAMP_TO_EDGE}
    };
}
Texture::Preset Texture::NearestRepeat() {
    return Preset {
        {gl::TEXTURE_MIN_FILTER, gl::NEAREST},
        {gl::TEXTURE_MAG_FILTER, gl::NEAREST},
        {gl::TEXTURE_WRAP_S, gl::REPEAT},
        {gl::TEXTURE_WRAP_T, gl::REPEAT},
        {gl::TEXTURE_WRAP_R, gl::REPEAT}
    };
}
Texture::Preset Texture::NearestMirror() {
    return Preset {
        {gl::TEXTURE_MIN_FILTER, gl::NEAREST},
        {gl::TEXTURE_MAG_FILTER, gl::NEAREST},
        {gl::TEXTURE_WRAP_S, gl::MIRRORED_REPEAT},
        {gl::TEXTURE_WRAP_T, gl::MIRRORED_REPEAT},
        {gl::TEXTURE_WRAP_R, gl::MIRRORED_REPEAT}
    };
}
Texture::Preset Texture::LinearClamp() {
    return Preset {
        {gl::TEXTURE_MIN_FILTER, gl::LINEAR},
        {gl::TEXTURE_MAG_FILTER, gl::LINEAR},
        {gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE},
        {gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE},
        {gl::TEXTURE_WRAP_R, gl::CLAMP_TO_EDGE}
    };
}
Texture::Preset Texture::LinearRepeat() {
    return Preset {
        {gl::TEXTURE_MIN_FILTER, gl::LINEAR},
        {gl::TEXTURE_MAG_FILTER, gl::LINEAR},
        {gl::TEXTURE_WRAP_S, gl::REPEAT},
        {gl::TEXTURE_WRAP_T, gl::REPEAT},
        {gl::TEXTURE_WRAP_R, gl::REPEAT}
    };
}
Texture::Preset Texture::LinearMirror() {
    return Preset {
        {gl::TEXTURE_MIN_FILTER, gl::LINEAR},
        {gl::TEXTURE_MAG_FILTER, gl::LINEAR},
        {gl::TEXTURE_WRAP_S, gl::MIRRORED_REPEAT},
        {gl::TEXTURE_WRAP_T, gl::MIRRORED_REPEAT},
        {gl::TEXTURE_WRAP_R, gl::MIRRORED_REPEAT}
    };
}
Texture::Preset Texture::MipmapClamp() {
    return Preset {
        {gl::TEXTURE_MIN_FILTER, gl::LINEAR_MIPMAP_LINEAR},
        {gl::TEXTURE_MAG_FILTER, gl::LINEAR},
        {gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE},
        {gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE},
        {gl::TEXTURE_WRAP_R, gl::CLAMP_TO_EDGE}
    };
}
Texture::Preset Texture::MipmapRepeat() {
    return Preset {
        {gl::TEXTURE_MIN_FILTER, gl::LINEAR_MIPMAP_LINEAR},
        {gl::TEXTURE_MAG_FILTER, gl::LINEAR},
        {gl::TEXTURE_WRAP_S, gl::REPEAT},
        {gl::TEXTURE_WRAP_T, gl::REPEAT},
        {gl::TEXTURE_WRAP_R, gl::REPEAT}
    };
}
Texture::Preset Texture::MipmapMirror() {
    return Preset {
        {gl::TEXTURE_MIN_FILTER, gl::LINEAR_MIPMAP_LINEAR},
        {gl::TEXTURE_MAG_FILTER, gl::LINEAR},
        {gl::TEXTURE_WRAP_S, gl::MIRRORED_REPEAT},
        {gl::TEXTURE_WRAP_T, gl::MIRRORED_REPEAT},
        {gl::TEXTURE_WRAP_R, gl::MIRRORED_REPEAT}
    };
}
Texture::Preset Texture::ShadowMap() {
    return Preset {
        {gl::TEXTURE_MIN_FILTER, gl::LINEAR},
        {gl::TEXTURE_MAG_FILTER, gl::LINEAR},
        {gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE},
        {gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE},
        {gl::TEXTURE_WRAP_R, gl::CLAMP_TO_EDGE},
        {gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE}
    };
}


// Resource Holder Singletons /////

ResHolder<Texture2D>& sq::res::tex2D() {
    static ResHolder<Texture2D> holder;
    return holder;
}

ResHolder<TextureCube>& sq::res::texCube() {
    static ResHolder<TextureCube> holder;
    return holder;
}
