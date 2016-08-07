#include <map>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/redist/stb_image.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/app/Resources.hpp>
#include <sqee/gl/Context.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/misc/Files.hpp>

using namespace sq;

// Helpers /////

struct LoadedImage {
    LoadedImage(int _w, int _h, int _c, uchar* _d) : width(_w), height(_h), channels(_c), data(_d) {}
    ~LoadedImage() { if (data != NULL) free(data); data = NULL; } int width, height, channels; uchar* data;
};

LoadedImage load_image(const string _path, GLenum _channels) {
    int width, height, channels, reqChannels;
    if      (_channels == 0u)       reqChannels = 0;
    else if (_channels == gl::RED)  reqChannels = 1;
    else if (_channels == gl::RG)   reqChannels = 2;
    else if (_channels == gl::RGB)  reqChannels = 3;
    else if (_channels == gl::RGBA) reqChannels = 4;
    else SQASSERT(false, "invalid channel format");

    uchar* data = stbi_load(_path.c_str(), &width, &height, &channels, reqChannels);
    if (data == NULL) return LoadedImage(0, 0, 0, NULL);
    if (reqChannels != 0) channels = reqChannels;

    uchar* top; uchar* btm; uchar temp;
    int widthBytes = width * channels;
    int halfHeight = height / 2;

    for (int y = 0; y < halfHeight; ++y) {
        top = data + y * widthBytes;
        btm = data + (height - y - 1) * widthBytes;
        for (int x = 0; x < widthBytes; ++x) {
            temp = *top; *top = *btm;
            *btm = temp; ++top; ++btm;
        }
    }

    return LoadedImage(width, height, channels, data);
}

Texture::Preset preset_from_zzz(const string& _path) {
    static const std::map<char, GLenum> WRAP_MAP {
        {'C', gl::CLAMP_TO_EDGE}, {'R', gl::REPEAT}, {'M', gl::MIRRORED_REPEAT}
    };

    static const std::map<char, GLenum> SWIZZLE_MAP {
        {'0', gl::ZERO}, {'1', gl::ONE}, {'R', gl::RED}, {'G', gl::GREEN}, {'B', gl::BLUE}, {'A', gl::ALPHA}
    };

    Texture::Preset preset; preset.reserve(8u);
    preset.emplace_back(gl::TEXTURE_MIN_FILTER, gl::LINEAR_MIPMAP_LINEAR);
    preset.emplace_back(gl::TEXTURE_MAG_FILTER, gl::LINEAR);

    const string textureName = file_name_from_path(_path);
    const string zzzPath = directory_from_path(_path) + "zzz.txt";

    try {
        for (const auto& linePair : sq::tokenise_file(zzzPath)) {
            const vector<string>& line = linePair.first;

            if (line.front() == textureName) {
                preset.emplace_back(gl::TEXTURE_WRAP_S, WRAP_MAP.at(line[1][0]));
                preset.emplace_back(gl::TEXTURE_WRAP_T, WRAP_MAP.at(line[1][1]));
                preset.emplace_back(gl::TEXTURE_SWIZZLE_R, SWIZZLE_MAP.at(line[2][0]));
                preset.emplace_back(gl::TEXTURE_SWIZZLE_G, SWIZZLE_MAP.at(line[2][1]));
                preset.emplace_back(gl::TEXTURE_SWIZZLE_B, SWIZZLE_MAP.at(line[2][2]));
                preset.emplace_back(gl::TEXTURE_SWIZZLE_A, SWIZZLE_MAP.at(line[2][3]));
            }
        }
    }
    catch (std::out_of_range) {
        log_error("invalid wrap or clamp mode for texture '%s'", _path);
    }

    if (preset.size() != 8u) {
        log_error("texture '%s' not found in %s", textureName, zzzPath);
    }

    return preset;
}


// sq::Texture /////

Texture::~Texture() {
    this->delete_object();
}

void Texture::delete_object() {
    if (tex != 0u) {
        static auto& context = Context::get();
        context.impl_delete_Texture(this);
        gl::DeleteTextures(1, &tex);
        tex = 0u; size = Vec3U(0u);
    }
}

void Texture::generate_mipmaps() const {
    gl::GenerateTextureMipmap(tex);
}

void Texture::create_and_setup_texture() {
    this->delete_object();
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
    else log_error("Failed to find texture %s", path);

    LoadedImage img = load_image(path, format);
    log_assert(img.data, "Failed to load texture from %s", path);
    buffer_memory(img.data, gl::UNSIGNED_BYTE);
}

void Texture2D::buffer_auto(const string& _path, bool _mipmaps) {
    string path = static_path() + "textures/" + _path;
    if (check_file_exists(path+".png")) path += ".png";
    else if (check_file_exists(path+".jpg")) path += ".jpg";
    else log_error("Failed to find texture %s", path);

    LoadedImage img = load_image(path, format);
    log_assert(img.data, "Failed to load texture from %s", path);
    allocate_storage(Vec2U(img.width, img.height), _mipmaps);
    buffer_memory(img.data, gl::UNSIGNED_BYTE);
    if (_mipmaps) generate_mipmaps();
}

void Texture2D::allocate_storage(Vec2U _size, bool _mipmaps) {
    create_and_setup_texture(); size = Vec3U(_size.x, _size.y, 0u);
    int maxLevel = _mipmaps ? std::floor(std::log2(std::max(size.x, size.y))) + 1 : 1;
    gl::TextureStorage2D(tex, maxLevel, iFormat, size.x, size.y);
}

unique_ptr<Texture2D> sq::load_Texture2D(const string& _path) {
    string path = static_path() + "textures/" + _path;
    if (check_file_exists(path+".png")) path += ".png";
    else if (check_file_exists(path+".jpg")) path += ".jpg";
    else log_error("Failed to find texture %s", path);

    LoadedImage img = load_image(path, 0u);
    log_assert(img.data, "Failed to load texture from %s", path);

    const array<GLenum, 5> formats {0u, gl::RED, gl::RG, gl::RGB, gl::RGBA};
    const array<GLenum, 5> iFormats {0u, gl::R8, gl::RG8, gl::RGB8, gl::RGBA8};
    const auto format = formats[img.channels], iFormat = iFormats[img.channels];
    const auto preset = preset_from_zzz(static_path() + "textures/" + _path);

    auto texture = std::make_unique<Texture2D>(format, iFormat, preset);
    texture->allocate_storage(Vec2U(img.width, img.height), true);
    texture->buffer_memory(img.data, gl::UNSIGNED_BYTE);
    texture->generate_mipmaps(); return texture;
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
    create_and_setup_texture(); size = Vec3U(_size, _size, 6u);
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
