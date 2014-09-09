#include <iostream>

#include <gl/textures.hpp>

#include <misc/stb_image.hpp>

using namespace sq;

Texture::Texture(GLenum _type, GLenum _format) {
    type = _type;
    format = _format;
    gl::GenTextures(1, &tex);
    std::cout << "created texture " << tex << std::endl;
}

Texture::~Texture() {
    std::cout << "Deleting texture " << tex << std::endl;
    gl::DeleteTextures(1, &tex);
}

const GLuint& Texture::get() {
    return tex;
}

void Texture::bind() {
    gl::BindTexture(type, tex);
}

void Texture::bind(GLenum _slot) {
    gl::ActiveTexture(_slot);
    gl::BindTexture(type, tex);
}

void Texture::set_param(GLenum _name, GLenum _value) {
    gl::TexParameteri(type, _name, _value);
}

void Texture::set_params(int num, const GLenum* _names, const GLenum* _values) {
    for (int i = 0; i < num; i++) {
        gl::TexParameteri(type, _names[i], _values[i]);
    }
}



Texture::Ptr sq::tex2D_load_blank(glm::uvec2 _size, GLenum _internalFormat) {
    Texture::Ptr tex(new Texture(gl::TEXTURE_2D, gl::RGBA));
    tex->size = {_size.x, _size.y, 1};
    tex->bind();
    gl::TexImage2D(gl::TEXTURE_2D, 0, _internalFormat, _size.x, _size.y,
                   0, gl::RGBA, gl::UNSIGNED_BYTE, nullptr);

    return tex;
}

Texture::Ptr sq::tex2D_load_memory(glm::uvec2 _size, GLenum _internalFormat, const unsigned char* _data) {
    Texture::Ptr tex(new Texture(gl::TEXTURE_2D, gl::RGBA));
    tex->size = {_size.x, _size.y, 1};
    tex->bind();
    gl::TexImage2D(gl::TEXTURE_2D, 0, _internalFormat, _size.x, _size.y,
                   0, gl::RGBA, gl::UNSIGNED_BYTE, _data);

    return tex;
}

Texture::Ptr sq::tex2D_load_file(std::string _path, GLenum _internalFormat) {
    Texture::Ptr tex(new Texture(gl::TEXTURE_2D, gl::RGBA));

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
        return nullptr;
    }
    tex->size = {w, h, 1};
    tex->bind();
    gl::TexImage2D(gl::TEXTURE_2D, 0, _internalFormat, w, h,
                   0, gl::RGBA, gl::UNSIGNED_BYTE, data);
    free(data);
    return tex;
}



Texture::Ptr sq::texDepth_load_blank(glm::uvec2 _size, GLenum _internalFormat) {
    Texture::Ptr tex(new Texture(gl::TEXTURE_2D, gl::DEPTH_COMPONENT));
    tex->size = {_size.x, _size.y, 1};
    tex->bind();
    gl::TexImage2D(gl::TEXTURE_2D, 0, _internalFormat, _size.x, _size.y,
                   0, gl::DEPTH_COMPONENT, gl::UNSIGNED_BYTE, nullptr);

    return tex;
}



Texture::Ptr sq::tex2DArray_load_blank(glm::uvec3 _size, GLenum _internalFormat) {
    Texture::Ptr tex(new Texture(gl::TEXTURE_2D_ARRAY, gl::RGBA));
    tex->size = _size;
    tex->bind();
    gl::TexImage3D(gl::TEXTURE_2D_ARRAY, 0, _internalFormat, _size.x, _size.y, _size.z,
                   0, gl::RGBA, gl::UNSIGNED_BYTE, nullptr);

    return tex;
}

bool sq::tex2DArray_add_file(Texture::Ptr _tex, std::string _path, GLuint _index) {
    int w, h, n;
    unsigned char* data = stbi_load(_path.c_str(), &w, &h, &n, 4);
    if (!data) {
        std::cout << "ERROR: Failed to load texture from \"" << _path << "\"" << std::endl;
        free(data);
        return true;
    }
    _tex->bind();
    gl::TexSubImage3D(gl::TEXTURE_2D_ARRAY, 0, 0, 0, _index, _tex->size.x, _tex->size.y, 1,
                      gl::RGBA, gl::UNSIGNED_BYTE, data);

    return false;
}
