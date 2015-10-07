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

Texture::~Texture() {
    gl::DeleteTextures(1, &tex);
}

void Texture::set_param(GLenum _name, GLenum _value) const {
    gl::TextureParameteri(target, _name, _value);
}

void Texture::set_preset(Preset _preset) const {
    for (const pair<GLenum, GLenum>& val : _preset)
        gl::TextureParameteri(tex, val.first, val.second);
}

void Texture::generate_mipmaps() const {
    gl::GenerateTextureMipmap(tex);
}

void Texture::bind(GLenum _slot) const {
    gl::ActiveTexture(_slot);
    gl::BindTexture(target, tex);
}

void Texture::bind() const {
    gl::BindTexture(target, tex);
}

uvec3 Texture::get_size() const {
    return tex ? size : uvec3(0u);
}


// sq::Texture2D /////

void Texture2D::create(GLenum _format, GLenum _iFormat, uint _channels, bool _mipmaps) {
    format = _format; iFormat = _iFormat; channels = _channels; mipmaps = _mipmaps;
    dataType = format == gl::DEPTH_STENCIL ? gl::UNSIGNED_INT_24_8 : gl::UNSIGNED_BYTE;
    target = gl::TEXTURE_2D; gl::CreateTextures(target, 1, &tex);
}

void Texture2D::buffer_memory(const void* _data) {
    gl::TextureSubImage2D(tex, 0, 0, 0, size.x, size.y, format, dataType, _data);
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
    size = uvec3(_size.x, _size.y, 0u);
    int maxLevel = mipmaps ? std::floor(std::log2(std::max(size.x, size.y))) + 1 : 1;
    gl::TextureStorage2D(tex, maxLevel, iFormat, size.x, size.y);
}


// sq::TextureMut2D /////

void TextureMut2D::create(GLenum _format, GLenum _iFormat, uint _channels) {
    format = _format; iFormat = _iFormat; channels = _channels;
    dataType = format == gl::DEPTH_STENCIL ? gl::UNSIGNED_INT_24_8 : gl::UNSIGNED_BYTE;
    target = gl::TEXTURE_2D; gl::CreateTextures(target, 1, &tex);
}

void TextureMut2D::buffer_memory(const void* _data) {
    gl::TextureSubImage2D(tex, 0, 0, 0, size.x, size.y, format, dataType, _data);
}

void TextureMut2D::resize(uvec2 _size) {
    gl::BindTexture(target, tex); size = uvec3(_size.x, _size.y, 0);
    gl::TexImage2D(target, 0, iFormat, size.x, size.y, 0, format, dataType, nullptr);
}


// sq::TextureCube /////

void TextureCube::create(GLenum _format, GLenum _iFormat, uint _channels, bool _mipmaps) {
    format = _format; iFormat = _iFormat; channels = _channels; mipmaps = _mipmaps;
    dataType = format == gl::DEPTH_STENCIL ? gl::UNSIGNED_INT_24_8 : gl::UNSIGNED_BYTE;
    target = gl::TEXTURE_CUBE_MAP; gl::CreateTextures(target, 1, &tex);
}

void TextureCube::buffer_memory(const void* _data, uint _face) {
    gl::TextureSubImage3D(tex, 0, 0, 0, _face, size.x, size.y, 1, format, dataType, _data);
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
    size = uvec3(_size, _size, 0u);
    int maxLevel = mipmaps ? std::floor(std::log2(size.x)) + 1 : 1;
    gl::TextureStorage2D(tex, maxLevel, iFormat, size.x, size.x);
}


// sq::TextureMutCube /////

void TextureMutCube::create(GLenum _format, GLenum _iFormat, uint _channels) {
    format = _format; iFormat = _iFormat; channels = _channels;
    dataType = format == gl::DEPTH_STENCIL ? gl::UNSIGNED_INT_24_8 : gl::UNSIGNED_BYTE;
    target = gl::TEXTURE_CUBE_MAP; gl::CreateTextures(target, 1, &tex);
}

void TextureMutCube::buffer_memory(const void* _data, uint _face) {
    gl::TextureSubImage3D(tex, 0, 0, 0, _face, size.x, size.y, 1, format, dataType, _data);
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
    target = gl::TEXTURE_2D_ARRAY; gl::CreateTextures(target, 1, &tex);
}

void Texture2DArray::buffer_memory(const void* _data, uint _index) {
    gl::TextureSubImage3D(tex, 0, 0, 0, _index, size.x, size.y, 1, format, dataType, _data);
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
    size = uvec3(_size.x, _size.y, _size.z);
    int maxLevel = mipmaps ? std::floor(std::log2(std::max(size.x, size.y))) + 1 : 1;
    gl::TextureStorage3D(tex, maxLevel, iFormat, size.x, size.y, size.z);
}


// sq::TextureMut2DArray /////

void TextureMut2DArray::create(GLenum _format, GLenum _iFormat, uint _channels) {
    format = _format; iFormat = _iFormat; channels = _channels;
    dataType = format == gl::DEPTH_STENCIL ? gl::UNSIGNED_INT_24_8 : gl::UNSIGNED_BYTE;
    target = gl::TEXTURE_2D_ARRAY; gl::CreateTextures(target, 1, &tex);
}

void TextureMut2DArray::buffer_memory(const void* _data, uint _index) {
    gl::TextureSubImage3D(tex, 0, 0, 0, _index, size.x, size.y, 1, format, dataType, _data);
}

void TextureMut2DArray::resize(uvec3 _size) {
    gl::BindTexture(target, tex); size = uvec3(_size.x, _size.y, _size.z);
    gl::TexImage3D(target, 0, iFormat, size.x, size.y, size.z, 0, format, dataType, nullptr);
}


// sq::TextureCubeArray /////

void TextureCubeArray::create(GLenum _format, GLenum _iFormat, uint _channels, bool _mipmaps) {
    format = _format; iFormat = _iFormat; channels = _channels; mipmaps = _mipmaps;
    dataType = format == gl::DEPTH_STENCIL ? gl::UNSIGNED_INT_24_8 : gl::UNSIGNED_BYTE;
    target = gl::TEXTURE_CUBE_MAP_ARRAY; gl::CreateTextures(target, 1, &tex);
}

void TextureCubeArray::buffer_memory(const void* _data, uint _face, uint _index) {
    gl::TextureSubImage3D(tex, 0, 0, 0, _index*6+_face, size.x, size.y, 1, format, dataType, _data);
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
    size = uvec3(_size.x, _size.x, _size.y);
    int maxLevel = mipmaps ? std::floor(std::log2(size.x)) + 1 : 1;
    gl::TextureStorage3D(tex, maxLevel, iFormat, size.x, size.y, size.z*6);
}


// sq::TextureMutCubeArray /////

void TextureMutCubeArray::create(GLenum _format, GLenum _iFormat, uint _channels) {
    format = _format; iFormat = _iFormat; channels = _channels;
    dataType = format == gl::DEPTH_STENCIL ? gl::UNSIGNED_INT_24_8 : gl::UNSIGNED_BYTE;
    target = gl::TEXTURE_CUBE_MAP_ARRAY; gl::CreateTextures(target, 1, &tex);
}

void TextureMutCubeArray::buffer_memory(const void* _data, uint _face, uint _index) {
    gl::TextureSubImage3D(tex, 0, 0, 0, _index*6+_face, size.x, size.y, 1, format, dataType, _data);
}

void TextureMutCubeArray::resize(uvec2 _size) {
    gl::BindTexture(target, tex); size = uvec3(_size.x, _size.x, _size.y);
    gl::TexImage3D(target, 0, iFormat, size.x, size.x, size.z*6, 0, format, dataType, nullptr);
}


// sq::TextureVolume /////

void TextureVolume::create(GLenum _format, GLenum _iFormat, uint _channels) {
    format = _format; iFormat = _iFormat; channels = _channels;
    dataType = gl::UNSIGNED_BYTE; target = gl::TEXTURE_3D;
    gl::CreateTextures(target, 1, &tex);
}

void TextureVolume::buffer_memory(const void* _data) {
//    gl::BindTexture(target, tex);
//    struct MD { float x, y, z, index; };
//    for (uint i = 0u; i < 12u*5u*5u; ++i) {
//        std::cout << i << " " << ((MD*)_data)[i].z << std::endl;
//    }
//    gl::TexSubImage3D(target, 0, 0, 0, 0, size.x, size.y, size.z, format, gl::FLOAT, _data);
//    //gl::TexImage3D(target, 0u, iFormat, size.x, size.y, size.z, 0, format, gl::FLOAT, _data);
}

void TextureVolume::allocate_storage(uvec3 _size) {
    size = uvec3(_size.x, _size.y, _size.z);
    gl::TextureStorage3D(tex, 1, iFormat, size.x, size.y, size.z);
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
