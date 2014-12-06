#include <gl/textures.hpp>
#include <redist/stb_image.hpp>

using namespace sq;

Texture::~Texture() {
    gl::DeleteTextures(1, &tex);
}

void Texture::create(GLenum _target, GLenum _format, GLenum _iFormat, Preset _preset) {
    #ifdef SQEE_DEBUG
    // Add something in here
    #endif
    target = _target;
    format = _format;
    iFormat = _iFormat;
    dataType = (format != gl::DEPTH_STENCIL) ?
                gl::UNSIGNED_BYTE : gl::UNSIGNED_INT_24_8;

    if      (iFormat == gl::DEPTH_COMPONENT16 || iFormat == gl::DEPTH_COMPONENT32 ||
             iFormat == gl::DEPTH24_STENCIL8 ||
             iFormat == gl::R8 || iFormat == gl::R16 || iFormat == gl::R32F) channels = 1;
    else if (iFormat == gl::RG8 || iFormat == gl::RG16 || iFormat == gl::RG32F) channels = 2;
    else if (iFormat == gl::RGB8 || iFormat == gl::RGB16 || iFormat == gl::RGB32F) channels = 3;
    else if (iFormat == gl::RGBA8 || iFormat == gl::RGBA16 || iFormat == gl::RGBA32F) channels = 4;
    else cout << "ERROR: Unsupported texture iFormat given" << endl;

    if (target == gl::TEXTURE_1D) dimensions = 1;
    else if (target == gl::TEXTURE_1D_ARRAY || target == gl::TEXTURE_2D) dimensions = 2;
    else if (target == gl::TEXTURE_2D_ARRAY || target == gl::TEXTURE_3D) dimensions = 3;
    else cout << "ERROR: Unsupported texture target given" << endl;

    gl::GenTextures(1, &tex);
    set_preset(_preset);
}

void Texture::create(GLenum _target, GLenum _format, GLenum _iFormat,
                     const string& _filePath, Preset _preset) {
    create(_target, _format, _iFormat, _preset);
    buffer_file(_filePath);
}

void Texture::resize(glm::uvec3 _size) {
    size = _size;
    bind();
    if (dimensions == 1)
        gl::TexImage1D(target, 0, iFormat, size.x, 0, format, dataType, nullptr);
    else if (dimensions == 2)
        gl::TexImage2D(target, 0, iFormat, size.x, size.y, 0, format, dataType, nullptr);
    else if (dimensions == 3)
        gl::TexImage3D(target, 0, iFormat, size.x, size.y, size.z, 0, format, dataType, nullptr);
}

void Texture::buffer_memory(const void* _data, uint _z) {
    bind();
    if      (dimensions == 1) gl::TexImage1D(target, 0, iFormat, size.x,
                                             0, format, dataType, _data);
    else if (dimensions == 2) gl::TexImage2D(target, 0, iFormat, size.x, size.y,
                                             0, format, dataType, _data);
    else if (dimensions == 3) gl::TexSubImage3D(target, 0, 0, 0, _z, size.x, size.y, 1,
                                               format, dataType, _data);
}

void Texture::buffer_file(const string& _filePath, uint _z) {
    string fPath = SQ_TEXTURES + _filePath + ".png";

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
    if (dimensions < 3) size = {w, h, 1};

    #ifdef SQEE_DEBUG
    if (!data)
        cout << "ERROR: Failed to load texture from \""
             << fPath << "\"" << endl;
    #endif

    bind();
    if (dimensions == 1)
        gl::TexImage1D(target, 0, iFormat, size.x, 0, format, dataType, data);
    else if (dimensions == 2)
        gl::TexImage2D(target, 0, iFormat, size.x, size.y, 0, format, dataType, data);
    else if (dimensions == 3)
        gl::TexSubImage3D(target, 0, 0, 0, _z, size.x, size.y, 1, format, dataType, data);

    free(data);
}

void Texture::set_preset(Preset _preset) {
    bind();
    if (_preset == Preset::L_C || _preset == Preset::L_R || _preset == Preset::SHAD) {
        gl::TexParameteri(target, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
        gl::TexParameteri(target, gl::TEXTURE_MAG_FILTER, gl::LINEAR);
    } else
    if (_preset == Preset::N_C || _preset == Preset::N_R) {
        gl::TexParameteri(target, gl::TEXTURE_MIN_FILTER, gl::NEAREST);
        gl::TexParameteri(target, gl::TEXTURE_MAG_FILTER, gl::NEAREST);
    }
    if (_preset == Preset::L_C || _preset == Preset::N_C || _preset == Preset::SHAD) {
        gl::TexParameteri(target, gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE);
        gl::TexParameteri(target, gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE);
    } else
    if (_preset == Preset::L_R || _preset == Preset::N_R) {
        gl::TexParameteri(target, gl::TEXTURE_WRAP_S, gl::REPEAT);
        gl::TexParameteri(target, gl::TEXTURE_WRAP_T, gl::REPEAT);
    }
    if (_preset == Preset::SHAD) {
        gl::TexParameteri(target, gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
    }
}

void Texture::bind(GLenum _slot) {
    if (_slot != gl::NONE) gl::ActiveTexture(_slot);
    gl::BindTexture(target, tex);
}

void Texture::set_param(GLenum _name, GLenum _value) {
    bind();
    gl::TexParameteri(target, _name, _value);
}
