#pragma once

#include <gl/gl.hpp>
#include <misc/containers.hpp>

#ifndef SQEE_TEX
#define SQEE_TEX "res/textures/"
#endif

namespace sq {

enum class TexPreset {
    NONE, N_C, N_R, L_C, L_R
};

class Texture {
public:
    ~Texture();

    // These are pretty crappy, compiler can't easily distinguish
    // the second and third ctors. FIX IT

    void create(GLenum _target, GLenum _format, GLenum _iFormat, glm::uvec3 _size,
                TexPreset _preset = TexPreset::NONE);
    void create(GLenum _target, GLenum _format, GLenum _iFormat, glm::uvec3 _size,
                const void* _data,
                TexPreset _preset = TexPreset::NONE);
    void create(GLenum _target, GLenum _format, GLenum _iFormat,
                const std::string& _filePath,
                TexPreset _preset = TexPreset::NONE);

    void resize(glm::uvec3 _size);
    void buffer_memory(const void* _data, int _z = -1);
    void buffer_file(const std::string& _filePath, int _z = -1);
    void set_preset(TexPreset _preset);

    void bind();
    void bind(GLenum _slot);
    void set_param(GLenum _name, GLenum _value);

    GLuint tex;
    GLenum target;
    GLenum format;
    GLenum iFormat;
    glm::uvec3 size;
private:
    int channels;
    int dimensions;
};
typedef ResHolder<std::string, Texture> TexHolder;

}
