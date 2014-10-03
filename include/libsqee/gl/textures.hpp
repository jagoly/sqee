#pragma once

#include <string>
#include <map>
#include <memory>

#include <gl/gl.hpp>

namespace sq {

const GLenum MIN_MAG_FILTERS[2]    = {gl::TEXTURE_MIN_FILTER, gl::TEXTURE_MAG_FILTER};
const GLenum BOTH_NEAREST[2]       = {gl::NEAREST, gl::NEAREST};
const GLenum BOTH_LINEAR[2]        = {gl::LINEAR, gl::LINEAR};
const GLenum S_T_WRAP[2]           = {gl::TEXTURE_WRAP_S, gl::TEXTURE_WRAP_T};
const GLenum BOTH_CLAMP_TO_EDGE[2] = {gl::CLAMP_TO_EDGE, gl::CLAMP_TO_EDGE};
const GLenum BOTH_REPEAT[2]        = {gl::REPEAT, gl::REPEAT};

 /////////////////////////

class Texture {
public:
    typedef std::shared_ptr<Texture> Ptr;

    Texture(GLenum _type, GLenum _format);
    ~Texture();

    glm::uvec3 size;

    void bind();
    void bind(GLenum _slot);
    void set_param(GLenum _name, GLenum _value);
    void set_params(int num, const GLenum* _names, const GLenum* _values);

    const GLuint& get();
private:
    GLuint tex;
    GLenum type;
    GLenum format;
};


Texture::Ptr tex_create(GLenum _type, GLenum _format);

Texture::Ptr tex2D_load_blank(glm::uvec2 _size, GLenum _internalFormat);
Texture::Ptr tex2D_load_memory(glm::uvec2 _size, GLenum _internalFormat, const unsigned char* _data);
Texture::Ptr tex2D_load_file(const std::string& _path, GLenum _internalFormat);

Texture::Ptr texDepth_load_blank(glm::uvec2 _size, GLenum _internalFormat);

Texture::Ptr tex2DArray_load_blank(glm::uvec3 _size, GLenum _internalFormat);
bool         tex2DArray_add_file(Texture::Ptr _tex, const std::string& _path, GLuint _index);

}
