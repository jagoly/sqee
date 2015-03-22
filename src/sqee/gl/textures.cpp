#include "app/logging.hpp"
#include "gl/gl_ext_3_3.hpp"
#include "misc/files.hpp"
#include "redist/stb_image.hpp"
#include "gl/textures.hpp"

using namespace sq;

Texture::~Texture() {
    gl::DeleteTextures(1, &tex);
}

void Texture::create(GLenum _target, GLenum _format, GLenum _iFormat, uint _channels, Preset _preset) {
    #ifdef SQEE_DEBUG
    // Add something in here
    #endif
    target = _target;
    format = _format;
    iFormat = _iFormat;
    channels = _channels;
    dataType = gl::UNSIGNED_BYTE;

    if (target == gl::TEXTURE_1D) dimensions = 1;
    else if (target == gl::TEXTURE_1D_ARRAY || target == gl::TEXTURE_2D) dimensions = 2;
    else if (target == gl::TEXTURE_2D_ARRAY || target == gl::TEXTURE_3D) dimensions = 3;
    else log_error("ERROR: Unsupported texture target given");

    gl::GenTextures(1, &tex);
    set_preset(_preset);
}

void Texture::resize(uvec3 _size) {
    size = _size;
    gl::BindTexture(target, tex);
    if (dimensions == 1)
        gl::TexImage1D(target, 0, iFormat, size.x, 0, format, dataType, nullptr);
    else if (dimensions == 2)
        gl::TexImage2D(target, 0, iFormat, size.x, size.y, 0, format, dataType, nullptr);
    else if (dimensions == 3)
        gl::TexImage3D(target, 0, iFormat, size.x, size.y, size.z, 0, format, dataType, nullptr);
}

void Texture::buffer_memory(const void* _data, uint _z) {
    gl::BindTexture(target, tex);
    if (dimensions == 1)
        gl::TexImage1D(target, 0, iFormat, size.x, 0, format, dataType, _data);
    else if (dimensions == 2)
        gl::TexImage2D(target, 0, iFormat, size.x, size.y, 0, format, dataType, _data);
    else if (dimensions == 3)
        gl::TexSubImage3D(target, 0, 0, 0, _z, size.x, size.y, 1, format, dataType, _data);
}

void Texture::buffer_file(const string& _path, uint _z) {
    string path = "res/textures/" + _path;
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
    if (dimensions < 3) size = {w, h, 1};

    #ifdef SQEE_DEBUG
    if (!data) log_error("Failed to load texture from $0", path);
    #endif

    buffer_memory(data, _z);
    free(data);
}

void Texture::set_preset(Preset _preset) {
    gl::BindTexture(target, tex);

    if (_preset == Preset::L_C || _preset == Preset::L_R || _preset == Preset::L_M) {
        gl::TexParameteri(target, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
        gl::TexParameteri(target, gl::TEXTURE_MAG_FILTER, gl::LINEAR);
    }
    if (_preset == Preset::N_C || _preset == Preset::N_R || _preset == Preset::N_M) {
        gl::TexParameteri(target, gl::TEXTURE_MIN_FILTER, gl::NEAREST);
        gl::TexParameteri(target, gl::TEXTURE_MAG_FILTER, gl::NEAREST);
    }
    if (_preset == Preset::L_C || _preset == Preset::N_C || _preset == Preset::M_C) {
        gl::TexParameteri(target, gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE);
        gl::TexParameteri(target, gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE);
    }
    if (_preset == Preset::L_R || _preset == Preset::N_R || _preset == Preset::M_R) {
        gl::TexParameteri(target, gl::TEXTURE_WRAP_S, gl::REPEAT);
        gl::TexParameteri(target, gl::TEXTURE_WRAP_T, gl::REPEAT);
    }
    if (_preset == Preset::L_M || _preset == Preset::N_M || _preset == Preset::M_M) {
        gl::TexParameteri(target, gl::TEXTURE_WRAP_S, gl::MIRRORED_REPEAT);
        gl::TexParameteri(target, gl::TEXTURE_WRAP_T, gl::MIRRORED_REPEAT);
    }
    if (_preset == Preset::M_C || _preset == Preset::M_R || _preset == Preset::M_M) {
        gl::TexParameteri(target, gl::TEXTURE_MIN_FILTER, gl::LINEAR_MIPMAP_LINEAR);
        gl::TexParameteri(target, gl::TEXTURE_MAG_FILTER, gl::LINEAR);
    }
}

void Texture::bind(GLenum _slot) {
    gl::ActiveTexture(_slot);
    gl::BindTexture(target, tex);
}

void Texture::bind() {
    gl::BindTexture(target, tex);
}

void Texture::set_param(GLenum _name, GLenum _value) {
    gl::BindTexture(target, tex);
    gl::TexParameteri(target, _name, _value);
}

void Texture::gen_mipmap() {
    gl::BindTexture(target, tex);
    gl::GenerateMipmap(target);
}


ResHolder<Texture>& sq::res::texture() {
    static ResHolder<Texture> holder;
    return holder;
}
