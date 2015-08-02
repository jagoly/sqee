#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/redist/stb_image.hpp>
#include <sqee/app/Logging.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/misc/Files.hpp>

using namespace sq;


Texture::~Texture() {
    gl::DeleteTextures(1, &tex);
}

void Texture::create_base() {
    gl::GenTextures(1, &tex);
}

void Texture::set_param(GLenum _name, GLenum _value) {
    gl::BindTexture(target, tex);
    gl::TexParameteri(target, _name, _value);
}

void Texture::set_preset(Preset _preset) {
    gl::BindTexture(target, tex);
    for (auto& val : _preset)
        gl::TexParameteri(target, val.first, val.second);
}

void Texture::bind(GLenum _slot) {
    gl::ActiveTexture(_slot);
    gl::BindTexture(target, tex);
}

void Texture::bind() {
    gl::BindTexture(target, tex);
}


void Texture2D::create(GLenum _format, GLenum _iFormat, uint _channels) {
    target = gl::TEXTURE_2D;
    format = _format;
    iFormat = _iFormat;
    channels = _channels;
    if (format == gl::DEPTH_STENCIL)
        dataType = gl::UNSIGNED_INT_24_8;
    else dataType = gl::UNSIGNED_BYTE;
    create_base();
}

void Texture2D::resize(uvec2 _size) {
    size = _size;
    gl::BindTexture(target, tex);
    gl::TexImage2D(target, 0, iFormat, size.x, size.y, 0, format, dataType, nullptr);
}

void Texture2D::buffer_memory(const void* _data) {
    gl::BindTexture(target, tex);
    gl::TexImage2D(target, 0, iFormat, size.x, size.y, 0, format, dataType, _data);
}

void Texture2D::buffer_file(const string& _path) {
    string path = res::path() + "textures/" + _path;
    if (check_file_exists(path+".png")) path += ".png";
    else if (check_file_exists(path+".jpg")) path += ".jpg";

    int w, h;
    uchar* data = stbi_load(path.c_str(), &w, &h, nullptr, channels);
    size = uvec2(w, h);
    uchar* top; uchar* btm; uchar temp;
    int wBytes = w * channels; int halfH = h / 2;
    for (int y = 0; y < halfH; y++) {
        top = data + y * wBytes;
        btm = data + (h - y - 1) * wBytes;
        for (int x = 0; x < wBytes; x++) {
            temp = *top; *top = *btm; *btm = temp;
            top++; btm++;
        }
    }

    if (!data) log_error("Failed to load texture from %s", path);

    buffer_memory(data);
    free(data);
}

void Texture2D::gen_mipmap() {
    gl::BindTexture(target, tex);
    gl::GenerateMipmap(target);
}

const Texture::Preset& Texture2D::N_C() {
    static const Preset preset = {
        {gl::TEXTURE_MIN_FILTER, gl::NEAREST},
        {gl::TEXTURE_MAG_FILTER, gl::NEAREST},
        {gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE},
        {gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE}
    }; return preset;
}
const Texture::Preset& Texture2D::N_R() {
    static const Preset preset = {
        {gl::TEXTURE_MIN_FILTER, gl::NEAREST},
        {gl::TEXTURE_MAG_FILTER, gl::NEAREST},
        {gl::TEXTURE_WRAP_S, gl::REPEAT},
        {gl::TEXTURE_WRAP_T, gl::REPEAT}
    }; return preset;
}
const Texture::Preset& Texture2D::N_M() {
    static const Preset preset = {
        {gl::TEXTURE_MIN_FILTER, gl::NEAREST},
        {gl::TEXTURE_MAG_FILTER, gl::NEAREST},
        {gl::TEXTURE_WRAP_S, gl::MIRRORED_REPEAT},
        {gl::TEXTURE_WRAP_T, gl::MIRRORED_REPEAT}
    }; return preset;
}
const Texture::Preset& Texture2D::L_C() {
    static const Preset preset = {
        {gl::TEXTURE_MIN_FILTER, gl::LINEAR},
        {gl::TEXTURE_MAG_FILTER, gl::LINEAR},
        {gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE},
        {gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE}
    }; return preset;
}
const Texture::Preset& Texture2D::L_R() {
    static const Preset preset = {
        {gl::TEXTURE_MIN_FILTER, gl::LINEAR},
        {gl::TEXTURE_MAG_FILTER, gl::LINEAR},
        {gl::TEXTURE_WRAP_S, gl::REPEAT},
        {gl::TEXTURE_WRAP_T, gl::REPEAT}
    }; return preset;
}
const Texture::Preset& Texture2D::L_M() {
    static const Preset preset = {
        {gl::TEXTURE_MIN_FILTER, gl::LINEAR},
        {gl::TEXTURE_MAG_FILTER, gl::LINEAR},
        {gl::TEXTURE_WRAP_S, gl::MIRRORED_REPEAT},
        {gl::TEXTURE_WRAP_T, gl::MIRRORED_REPEAT}
    }; return preset;
}
const Texture::Preset& Texture2D::M_C() {
    static const Preset preset = {
        {gl::TEXTURE_MIN_FILTER, gl::LINEAR_MIPMAP_LINEAR},
        {gl::TEXTURE_MAG_FILTER, gl::LINEAR},
        {gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE},
        {gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE}
    }; return preset;
}
const Texture::Preset& Texture2D::M_R() {
    static const Preset preset = {
        {gl::TEXTURE_MIN_FILTER, gl::LINEAR_MIPMAP_LINEAR},
        {gl::TEXTURE_MAG_FILTER, gl::LINEAR},
        {gl::TEXTURE_WRAP_S, gl::REPEAT},
        {gl::TEXTURE_WRAP_T, gl::REPEAT}
    }; return preset;
}
const Texture::Preset& Texture2D::M_M() {
    static const Preset preset = {
        {gl::TEXTURE_MIN_FILTER, gl::LINEAR_MIPMAP_LINEAR},
        {gl::TEXTURE_MAG_FILTER, gl::LINEAR},
        {gl::TEXTURE_WRAP_S, gl::MIRRORED_REPEAT},
        {gl::TEXTURE_WRAP_T, gl::MIRRORED_REPEAT}
    }; return preset;
}


void TextureCube::create(GLenum _format, GLenum _iFormat, uint _channels) {
    target = gl::TEXTURE_CUBE_MAP;
    dataType = gl::UNSIGNED_BYTE;
    format = _format;
    iFormat = _iFormat;
    channels = _channels;
    create_base();
}

void TextureCube::resize(uint _size) {
    size = _size;
    gl::BindTexture(target, tex);
    gl::TexImage2D(gl::TEXTURE_CUBE_MAP_POSITIVE_X, 0, iFormat, size, size, 0, format, dataType, nullptr);
    gl::TexImage2D(gl::TEXTURE_CUBE_MAP_NEGATIVE_X, 0, iFormat, size, size, 0, format, dataType, nullptr);
    gl::TexImage2D(gl::TEXTURE_CUBE_MAP_POSITIVE_Y, 0, iFormat, size, size, 0, format, dataType, nullptr);
    gl::TexImage2D(gl::TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, iFormat, size, size, 0, format, dataType, nullptr);
    gl::TexImage2D(gl::TEXTURE_CUBE_MAP_POSITIVE_Z, 0, iFormat, size, size, 0, format, dataType, nullptr);
    gl::TexImage2D(gl::TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, iFormat, size, size, 0, format, dataType, nullptr);
}

void TextureCube::buffer_memory(const void* _data, uint _face) {
    gl::BindTexture(target, tex);
    uint face = gl::TEXTURE_CUBE_MAP_POSITIVE_X + _face;
    gl::TexImage2D(face, 0, iFormat, size, size, 0, format, dataType, _data);
}

void TextureCube::buffer_file(const string& _path, uint _face) {
    string path = res::path() + "textures/" + _path;
    if (check_file_exists(path+".png")) path += ".png";
    else if (check_file_exists(path+".jpg")) path += ".jpg";

    int w, h;
    uchar* data = stbi_load(path.c_str(), &w, &h, nullptr, channels);
    size = w;
    uchar* top; uchar* btm; uchar temp;
    int wBytes = w * channels; int halfH = h / 2;

    for (int y = 0; y < halfH; y++) {
        top = data + y * wBytes;
        btm = data + (h - y - 1) * wBytes;
        for (int x = 0; x < wBytes; x++) {
            temp = *top; *top = *btm; *btm = temp;
            top++; btm++;
        }
    }

    if (!data) log_error("Failed to load texture from %s", path);

    buffer_memory(data, _face);
    free(data);
}

void TextureCube::buffer_full(const string& _path, uint _size) {
    buffer_file(_path+"/x+", 0);
    buffer_file(_path+"/x-", 1);
    buffer_file(_path+"/y+", 2);
    buffer_file(_path+"/y-", 3);
    buffer_file(_path+"/z+", 4);
    buffer_file(_path+"/z-", 5);
}

const Texture::Preset& TextureCube::N_C() {
    static const Preset preset = {
        {gl::TEXTURE_MIN_FILTER, gl::NEAREST},
        {gl::TEXTURE_MAG_FILTER, gl::NEAREST},
        {gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE},
        {gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE},
        {gl::TEXTURE_WRAP_R, gl::CLAMP_TO_EDGE}
    }; return preset;
}
const Texture::Preset& TextureCube::L_C() {
    static const Preset preset = {
        {gl::TEXTURE_MIN_FILTER, gl::LINEAR},
        {gl::TEXTURE_MAG_FILTER, gl::LINEAR},
        {gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE},
        {gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE},
        {gl::TEXTURE_WRAP_R, gl::CLAMP_TO_EDGE}
    }; return preset;
}


void TextureArray::create(GLenum _format, GLenum _iFormat, uint _channels) {
    target = gl::TEXTURE_2D_ARRAY;
    dataType = gl::UNSIGNED_BYTE;
    format = _format;
    iFormat = _iFormat;
    channels = _channels;
    create_base();
}

void TextureArray::resize(uvec3 _size) {
    size = _size;
    gl::BindTexture(target, tex);
    gl::TexImage3D(target, 0, iFormat, size.x, size.y, size.z, 0, format, dataType, nullptr);
}

void TextureArray::buffer_memory(const void* _data, uint _z) {
    gl::BindTexture(target, tex);
    gl::TexSubImage3D(target, 0, 0, 0, _z, size.x, size.y, 1, format, dataType, _data);
}

void TextureArray::buffer_file(const string& _path, uint _z) {
    string path = res::path() + "textures/" + _path;
    if (check_file_exists(path+".png")) path += ".png";
    else if (check_file_exists(path+".jpg")) path += ".jpg";

    int w, h;
    uchar* data = stbi_load(path.c_str(), &w, &h, nullptr, channels);
    uchar* top; uchar* btm; uchar temp;
    int wBytes = w * channels; int halfH = h / 2;
    for (int y = 0; y < halfH; y++) {
        top = data + y * wBytes;
        btm = data + (h - y - 1) * wBytes;
        for (int x = 0; x < wBytes; x++) {
            temp = *top; *top = *btm; *btm = temp;
            top++; btm++;
        }
    }

    if (!data) log_error("Failed to load texture from %s", path);

    buffer_memory(data, _z);
    free(data);
}

const Texture::Preset& TextureArray::N_C() {
    return Texture2D::N_C();
}
const Texture::Preset& TextureArray::N_R() {
    return Texture2D::N_R();
}
const Texture::Preset& TextureArray::N_M() {
    return Texture2D::N_M();
}
const Texture::Preset& TextureArray::L_C() {
    return Texture2D::L_C();
}
const Texture::Preset& TextureArray::L_R() {
    return Texture2D::L_R();
}
const Texture::Preset& TextureArray::L_M() {
    return Texture2D::L_M();
}


ResHolder<Texture2D>& sq::res::tex2D() {
    static ResHolder<Texture2D> holder;
    return holder;
}

ResHolder<TextureCube>& sq::res::texCube() {
    static ResHolder<TextureCube> holder;
    return holder;
}
