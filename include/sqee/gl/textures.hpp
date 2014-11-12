#pragma once

#include <gl/gl.hpp>
#include <misc/containers.hpp>

namespace sq {

enum class TexPreset {
    NONE, N_C, N_R, L_C, L_R
};

class Texture {
public:
    ~Texture();

    void create(GLenum _target, GLenum _format, GLenum _iFormat,
                TexPreset _preset = TexPreset::NONE);
    void create(GLenum _target, GLenum _format, GLenum _iFormat,
                const string& _filePath,
                TexPreset _preset = TexPreset::NONE);

    void resize(glm::uvec3 _size);
    void buffer_memory(const void* _data, uint _z = 0);
    void buffer_file(const string& _filePath, uint _z = 0);
    void set_preset(TexPreset _preset);

    void bind(GLenum _slot = gl::NONE);
    void set_param(GLenum _name, GLenum _value);

    GLuint tex;
    GLenum target;
    GLenum format;
    GLenum iFormat;
    glm::uvec3 size;

private:
    int channels;
    int dimensions;
    GLenum dataType;
};
typedef ResHolder<string, Texture> TexHolder;

}
