#include <iostream>

#include <gl/textures.hpp>

#include <misc/stb_image.hpp>

using namespace sq;


 /////////////////////////
 ///  TextureBase      ///
 /////////////////////////

TextureBase::TextureBase(GLenum _type, GLenum _format) {
    type = _type;
    format = _format;
    gl::GenTextures(1, &tex);
}

TextureBase::~TextureBase() { // FIX ME (copy ctor)
    //gl::DeleteTextures(1, &tex);
}

void TextureBase::bind() {
    gl::BindTexture(type, tex);
}

void TextureBase::bind(GLenum _slot) {
    gl::ActiveTexture(_slot);
    gl::BindTexture(type, tex);
}

void TextureBase::set_param(GLenum _name, GLenum _value) {
    gl::TexParameteri(type, _name, _value);
}

void TextureBase::set_params(int num, const GLenum* _names, const GLenum* _values) {
    for (int i = 0; i < num; i++) {
        gl::TexParameteri(type, _names[i], _values[i]);
    }
}


 /////////////////////////
 ///  Texture2D        ///
 /////////////////////////

void Texture2D::load_blank(glm::uvec2 _size, GLenum _internalFormat) {
    size = {_size.x, _size.y, 1};
    bind();
    gl::TexImage2D(gl::TEXTURE_2D, 0, _internalFormat, _size.x, _size.y,
                   0, gl::RGBA, gl::UNSIGNED_BYTE, NULL);
}

void Texture2D::load_from_memory(glm::uvec2 _size, GLenum _internalFormat, const unsigned char* _data) {
    size = {_size.x, _size.y, 1};
    bind();
    gl::TexImage2D(gl::TEXTURE_2D, 0, _internalFormat, _size.x, _size.y,
                   0, gl::RGBA, gl::UNSIGNED_BYTE, _data);
}

bool Texture2D::load_from_file(std::string _path, GLenum _internalFormat) {
    int w, h, n;
    unsigned char* data = stbi_load(_path.c_str(), &w, &h, &n, 4);
    int widthBytes = w * 4;
    unsigned char* top = nullptr;
    unsigned char* bottom = nullptr;
    unsigned char temp = 0;
    int halfH = h / 2;
    for (int y = 0; y < halfH; y++) {
        top = data + y * widthBytes;
        bottom = data + (h - y - 1) * widthBytes;
        for (int x = 0; x < widthBytes; x++) {
            temp = *top;
            *top = *bottom;
            *bottom = temp;
            top++;
            bottom++;
        }
    }

    if (!data) {
        std::cout << "Error loading texture from \"" << _path << "\"" << std::endl;
        free(data);
        return true;
    }
    size = {w, h, 1};
    bind();
    gl::TexImage2D(gl::TEXTURE_2D, 0, _internalFormat, w, h,
                   0, gl::RGBA, gl::UNSIGNED_BYTE, data);
    free(data);
    return false;
}


 /////////////////////////
 ///  TextureDepth     ///
 /////////////////////////

void TextureDepth::load_blank(glm::uvec2 _size, GLenum _internalFormat) {
    size = {_size.x, _size.y, 1};
    bind();
    gl::TexImage2D(gl::TEXTURE_2D, 0, _internalFormat, _size.x, _size.y,
                   0, gl::DEPTH_COMPONENT, gl::UNSIGNED_BYTE, NULL);
}


 /////////////////////////
 ///  Texture2DArray   ///
 /////////////////////////

void Texture2DArray::load_blank(glm::uvec3 _size, GLenum _internalFormat) {
    size = _size;
    bind();
    gl::TexImage3D(gl::TEXTURE_2D_ARRAY, 0, _internalFormat, size.x, size.y, size.z,
                   0, gl::RGBA, gl::UNSIGNED_BYTE, NULL);
}

bool Texture2DArray::load_from_file(std::string _path, GLuint _index) {
    int w, h, n;
    unsigned char* data = stbi_load(_path.c_str(), &w, &h, &n, 4);
    if (!data) {
        std::cout << "ERROR: Failed to load texture from \"" << _path << "\"" << std::endl;
        free(data);
        return true;
    }
    bind();
    gl::TexSubImage3D(gl::TEXTURE_2D_ARRAY, 0, 0, 0, _index, size.x, size.y, 1,
                      gl::RGBA, gl::UNSIGNED_BYTE, data);
    free(data);
    return false;
}
