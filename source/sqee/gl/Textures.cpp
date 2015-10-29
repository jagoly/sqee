#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/redist/stb_image.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/app/Resources.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/misc/Files.hpp>

using namespace sq;

// Helpers /////

struct LoadedImage {
    LoadedImage(uint _w, uint _h, uchar* _d) : width(_w), height(_h), data(_d) {}
    int width = 0; int height = 0; uchar* data = NULL;
    ~LoadedImage() { if (data != NULL) free(data); }
};

LoadedImage load_image(const string _path, GLenum _channels) {
    uint channels = 0u;
    if (_channels == gl::RED) channels = 1u;
    else if (_channels == gl::RG) channels = 2u;
    else if (_channels == gl::RGB) channels = 3u;
    else if (_channels == gl::RGBA) channels = 4u;

    int width = 0; int height = 0; uchar* data = NULL;
    data = stbi_load(_path.c_str(), &width, &height, 0, channels);
    uchar* top; uchar* btm; uchar temp; int wBytes = width * channels;

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

void Texture::delete_object() {
    if (tex != 0u) {
        gl::DeleteTextures(1, &tex);
        tex = 0u; size = Vec3U(0u, 0u, 0u);
    }
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

Vec3U Texture::get_size() const {
    return size;
}

void Texture::create_and_setup_texture() {
    if (tex != 0u) gl::DeleteTextures(1, &tex);
    gl::CreateTextures(target, 1, &tex);

    for (const pair<GLenum, GLenum>& val : preset)
        gl::TextureParameteri(tex, val.first, val.second);
}


// sq::Texture2D /////

Texture2D::Texture2D(GLenum _format, GLenum _iFormat, Preset _preset)
    : Texture(gl::TEXTURE_2D, _format, _iFormat, _preset) {}

void Texture2D::buffer_memory(const void* _data, GLenum _type) {
    gl::TextureSubImage2D(tex, 0, 0, 0, size.x, size.y, format, _type, _data);
}

void Texture2D::buffer_file(const string& _path) {
    string path = static_path() + "textures/" + _path;
    if (check_file_exists(path+".png")) path += ".png";
    else if (check_file_exists(path+".jpg")) path += ".jpg";

    LoadedImage img = load_image(path, format);
    if (!img.data) log_error("Failed to load texture from %s", path);
    else buffer_memory(img.data, gl::UNSIGNED_BYTE);
}

void Texture2D::buffer_auto(const string& _path, bool _mipmaps) {
    string path = static_path() + "textures/" + _path;
    if (check_file_exists(path+".png")) path += ".png";
    else if (check_file_exists(path+".jpg")) path += ".jpg";

    LoadedImage img = load_image(path, format);
    if (!img.data) log_error("Failed to load texture from %s", path);
    else { allocate_storage(Vec2U(img.width, img.height), _mipmaps);
           buffer_memory(img.data, gl::UNSIGNED_BYTE);
           if (_mipmaps) generate_mipmaps(); }
}

void Texture2D::allocate_storage(Vec2U _size, bool _mipmaps) {
    create_and_setup_texture(); size = Vec3U(_size.x, _size.y, 0u);
    int maxLevel = _mipmaps ? std::floor(std::log2(std::max(size.x, size.y))) + 1 : 1;
    gl::TextureStorage2D(tex, maxLevel, iFormat, size.x, size.y);
}


// sq::TextureCube /////

TextureCube::TextureCube(GLenum _format, GLenum _iFormat, Preset _preset)
    : Texture(gl::TEXTURE_CUBE_MAP, _format, _iFormat, _preset) {}

void TextureCube::buffer_memory(const void* _data, uint _face, GLenum _type) {
    gl::TextureSubImage3D(tex, 0, 0, 0, _face, size.x, size.y, 1, format, _type, _data);
}

void TextureCube::buffer_file(const string& _path, uint _face) {
    string path = static_path() + "textures/" + _path;
    if (check_file_exists(path+".png")) path += ".png";
    else if (check_file_exists(path+".jpg")) path += ".jpg";

    LoadedImage img = load_image(path, format);
    if (!img.data) log_error("Failed to load texture from %s", path);
    else { buffer_memory(img.data, _face, gl::UNSIGNED_BYTE); }
}

void TextureCube::buffer_full(const string& _path) {
    buffer_file(_path + "/x+", 0u);
    buffer_file(_path + "/x-", 1u);
    buffer_file(_path + "/y+", 2u);
    buffer_file(_path + "/y-", 3u);
    buffer_file(_path + "/z+", 4u);
    buffer_file(_path + "/z-", 5u);
}

void TextureCube::allocate_storage(uint _size, bool _mipmaps) {
    create_and_setup_texture(); size = Vec3U(_size, _size, 0u);
    int maxLevel = _mipmaps ? std::floor(std::log2(size.x)) + 1 : 1;
    gl::TextureStorage2D(tex, maxLevel, iFormat, size.x, size.x);
}


// sq::Texture2DArray /////

Texture2DArray::Texture2DArray(GLenum _format, GLenum _iFormat, Preset _preset)
    : Texture(gl::TEXTURE_2D_ARRAY, _format, _iFormat, _preset) {}

void Texture2DArray::buffer_memory(const void* _data, uint _index, GLenum _type) {
    gl::TextureSubImage3D(tex, 0, 0, 0, _index, size.x, size.y, 1, format, _type, _data);
}

void Texture2DArray::buffer_file(const string& _path, uint _index) {
    string path = static_path() + "textures/" + _path;
    if (check_file_exists(path+".png")) path += ".png";
    else if (check_file_exists(path+".jpg")) path += ".jpg";

    LoadedImage img = load_image(path, format);
    if (!img.data) log_error("Failed to load texture from %s", path);
    else buffer_memory(img.data, _index, gl::UNSIGNED_BYTE);
}

void Texture2DArray::allocate_storage(Vec3U _size, bool _mipmaps) {
    create_and_setup_texture(); size = Vec3U(_size.x, _size.y, _size.z);
    int maxLevel = _mipmaps ? std::floor(std::log2(std::max(size.x, size.y))) + 1 : 1;
    gl::TextureStorage3D(tex, maxLevel, iFormat, size.x, size.y, size.z);
}


// sq::TextureCubeArray /////

TextureCubeArray::TextureCubeArray(GLenum _format, GLenum _iFormat, Preset _preset)
    : Texture(gl::TEXTURE_CUBE_MAP_ARRAY, _format, _iFormat, _preset) {}

void TextureCubeArray::buffer_memory(const void* _data, uint _face, uint _index, GLenum _type) {
    gl::TextureSubImage3D(tex, 0, 0, 0, _index*6+_face, size.x, size.y, 1, format, _type, _data);
}

void TextureCubeArray::buffer_file(const string& _path, uint _face, uint _index) {
    string path = static_path() + "textures/" + _path;
    if (check_file_exists(path+".png")) path += ".png";
    else if (check_file_exists(path+".jpg")) path += ".jpg";

    LoadedImage img = load_image(path, format);
    if (!img.data) log_error("Failed to load texture from %s", path);
    else buffer_memory(img.data, _face, _index, gl::UNSIGNED_BYTE);
}

void TextureCubeArray::buffer_full(const string& _path, uint _index) {
    buffer_file(_path+"/x+", 0, _index);
    buffer_file(_path+"/x-", 1, _index);
    buffer_file(_path+"/y+", 2, _index);
    buffer_file(_path+"/y-", 3, _index);
    buffer_file(_path+"/z+", 4, _index);
    buffer_file(_path+"/z-", 5, _index);
}

void TextureCubeArray::allocate_storage(Vec2U _size, bool _mipmaps) {
    create_and_setup_texture(); size = Vec3U(_size.x, _size.x, _size.y);
    int maxLevel = _mipmaps ? std::floor(std::log2(size.x)) + 1 : 1;
    gl::TextureStorage3D(tex, maxLevel, iFormat, size.x, size.y, size.z*6);
}


// sq::TextureVolume /////

TextureVolume::TextureVolume(GLenum _format, GLenum _iFormat, Preset _preset)
    : Texture(gl::TEXTURE_3D, _format, _iFormat, _preset) {}

void TextureVolume::buffer_memory(const void* _data, GLenum _type) {
//    gl::BindTexture(target, tex);
//    struct MD { float x, y, z, index; };
//    for (uint i = 0u; i < 12u*5u*5u; ++i) {
//        std::cout << i << " " << ((MD*)_data)[i].z << std::endl;
//    }
//    gl::TexSubImage3D(target, 0, 0, 0, 0, size.x, size.y, size.z, format, gl::FLOAT, _data);
//    //gl::TexImage3D(target, 0u, iFormat, size.x, size.y, size.z, 0, format, gl::FLOAT, _data);
}

void TextureVolume::allocate_storage(Vec3U _size) {
    create_and_setup_texture(); size = Vec3U(_size.x, _size.y, _size.z);
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
