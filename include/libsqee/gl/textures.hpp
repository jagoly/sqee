#pragma once

#include <string>
#include <map>

#include <gl/gl.hpp>

namespace sq {

const GLenum MIN_MAG_FILTERS[2]    = {gl::TEXTURE_MIN_FILTER, gl::TEXTURE_MAG_FILTER};
const GLenum BOTH_NEAREST[2]       = {gl::NEAREST, gl::NEAREST};
const GLenum BOTH_LINEAR[2]        = {gl::LINEAR, gl::LINEAR};
const GLenum S_T_WRAP[2]           = {gl::TEXTURE_WRAP_S, gl::TEXTURE_WRAP_T};
const GLenum BOTH_CLAMP_TO_EDGE[2] = {gl::CLAMP_TO_EDGE,  gl::CLAMP_TO_EDGE};

 /////////////////////////

struct TextureBase {
    TextureBase(GLenum _type, GLenum _format);
    ~TextureBase();

    GLuint tex;
    GLenum type;
    GLenum format;
    glm::uvec3 size;

    void bind();
    void bind(GLenum _slot);
    void set_param(GLenum _name, GLenum _value);
    void set_params(int num, const GLenum* _names, const GLenum* _values);
};


 /////////////////////////

struct Texture2D : public TextureBase {
    Texture2D() : TextureBase(gl::TEXTURE_2D, gl::RGBA) {}

    void load_blank(glm::uvec2 _size, GLenum _internalFormat);
    void load_from_memory(glm::uvec2 _size, GLenum _internalFormat, const unsigned char* _data);
    bool load_from_file(std::string _path, GLenum _internalFormat);
};


 /////////////////////////

struct TextureDepth : public TextureBase {
    TextureDepth() : TextureBase(gl::TEXTURE_2D, gl::DEPTH_COMPONENT) {}

    void load_blank(glm::uvec2 _size, GLenum _internalFormat);
};


 /////////////////////////

struct Texture2DArray : public TextureBase {
    Texture2DArray() : TextureBase(gl::TEXTURE_2D_ARRAY, gl::RGBA) {}

    void load_blank(glm::uvec3 _size, GLenum _internalFormat);
    bool load_from_file(std::string _path, GLuint _index);
};

}
