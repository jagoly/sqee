#include <iostream>

#include <gl/textures.hpp>

#include <redist/stb_image.hpp>

using namespace sq;

Texture::~Texture() {
    gl::DeleteTextures(1, &tex);
}

void Texture::create(GLenum _target, GLenum _format, GLenum _iFormat,
                     glm::uvec3 _size, TexPreset _preset) {
#ifdef SQEE_DEBUG
    // Add something in here
#endif
    target = _target;
    format = _format;
    iFormat = _iFormat;

    if      (iFormat == gl::DEPTH_COMPONENT16 || iFormat == gl::DEPTH_COMPONENT32 ||
             iFormat == gl::R8 || iFormat == gl::R16 || iFormat == gl::R32F) channels = 1;
    else if (iFormat == gl::RG8 || iFormat == gl::RG16 || iFormat == gl::RG32F) channels = 2;
    else if (iFormat == gl::RGB8 || iFormat == gl::RGB16 || iFormat == gl::RGB32F) channels = 3;
    else if (iFormat == gl::RGBA8 || iFormat == gl::RGBA16 || iFormat == gl::RGBA32F) channels = 4;
    else std::cout << "ERROR: Unsupported texture iFormat given" << std::endl;

    if (target == gl::TEXTURE_1D) dimensions = 1;
    else if (target == gl::TEXTURE_1D_ARRAY || target == gl::TEXTURE_2D) dimensions = 2;
    else if (target == gl::TEXTURE_2D_ARRAY || target == gl::TEXTURE_3D) dimensions = 3;
    else std::cout << "ERROR: Unsupported texture target given" << std::endl;

    gl::GenTextures(1, &tex);
    set_preset(_preset);
    resize(_size);
}

void Texture::create(GLenum _target, GLenum _format, GLenum _iFormat, glm::uvec3 _size,
                     const void *_data, TexPreset _preset) {
    create(_target, _format, _iFormat, _size, _preset);
    buffer_memory(_data);
}

void Texture::create(GLenum _target, GLenum _format, GLenum _iFormat,
                     const std::string& _filePath, TexPreset _preset) {
    create(_target, _format, _iFormat, glm::uvec3(), _preset);
    buffer_file(_filePath);
}

void Texture::resize(glm::uvec3 _size) {
    size = _size;
    bind();
    if (dimensions == 1)
        gl::TexImage1D(target, 0, iFormat, size.x, 0, format, gl::UNSIGNED_BYTE, nullptr);
    else if (dimensions == 2)
        gl::TexImage2D(target, 0, iFormat, size.x, size.y, 0, format, gl::UNSIGNED_BYTE, nullptr);
    else if (dimensions == 3)
        gl::TexImage3D(target, 0, iFormat, size.x, size.y, size.z, 0, format, gl::UNSIGNED_BYTE, nullptr);
}

void Texture::buffer_memory(const void* _data, int _z) {
    bind();
    if      (dimensions == 1) gl::TexImage1D(target, 0, iFormat, size.x,
                                             0, format, gl::UNSIGNED_BYTE, _data);
    else if (dimensions == 2) gl::TexImage2D(target, 0, iFormat, size.x, size.y,
                                             0, format, gl::UNSIGNED_BYTE, _data);
    else if (dimensions == 3) {
        if (_z == -1) gl::TexImage3D(target, 0, iFormat, size.x, size.y, size.z,
                                    0, format, gl::UNSIGNED_BYTE, _data);
        else gl::TexSubImage3D(target, 0, 0, 0, _z, size.x, size.y, 1,
                               format, gl::UNSIGNED_BYTE, _data);
    }
}

void Texture::buffer_file(const std::string& _filePath, int _z) {
    std::string fPath = SQEE_TEX + _filePath + ".png";

    int w, h;
    uchar* data = stbi_load(fPath.c_str(), &w, &h, nullptr, channels);
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
#ifdef SQEE_DEBUG
    if (!data) std::cout << "ERROR: Failed to load texture from \""
                         << fPath << "\"" << std::endl;
#endif
    if (dimensions < 3) size = {w, h, 1};

    bind();
    if (dimensions == 1)
        gl::TexImage1D(target, 0, iFormat, size.x, 0, format, gl::UNSIGNED_BYTE, data);
    else if (dimensions == 2)
        gl::TexImage2D(target, 0, iFormat, size.x, size.y, 0, format, gl::UNSIGNED_BYTE, data);
    else if (dimensions == 3)
        gl::TexSubImage3D(target, 0, 0, 0, _z, size.x, size.y, 1, format, gl::UNSIGNED_BYTE, data);

    free(data);
}

void Texture::set_preset(TexPreset _preset) {
    bind();
    if (_preset == TexPreset::L_C || _preset == TexPreset::L_R) {
        gl::TexParameteri(target, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
        gl::TexParameteri(target, gl::TEXTURE_MAG_FILTER, gl::LINEAR);
    } else
    if (_preset == TexPreset::N_C || _preset == TexPreset::N_R) {
        gl::TexParameteri(target, gl::TEXTURE_MIN_FILTER, gl::NEAREST);
        gl::TexParameteri(target, gl::TEXTURE_MAG_FILTER, gl::NEAREST);
    }
    if (_preset == TexPreset::L_C || _preset == TexPreset::N_C) {
        gl::TexParameteri(target, gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE);
        gl::TexParameteri(target, gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE);
    } else
    if (_preset == TexPreset::L_R || _preset == TexPreset::N_R) {
        gl::TexParameteri(target, gl::TEXTURE_WRAP_S, gl::REPEAT);
        gl::TexParameteri(target, gl::TEXTURE_WRAP_T, gl::REPEAT);
    }
}

void Texture::bind() {
    gl::BindTexture(target, tex);
}

void Texture::bind(GLenum _slot) {
    gl::ActiveTexture(_slot);
    bind();
}

void Texture::set_param(GLenum _name, GLenum _value) {
    bind();
    gl::TexParameteri(target, _name, _value);
}
