#pragma once
#include "forward.hpp"

namespace sq {

class Texture : NonCopyable {
public:
    enum class Preset { NONE, N_C, N_R, L_C, L_R, M_C, M_R, SHAD };

    ~Texture();

    void create(GLenum _target, GLenum _format, GLenum _iFormat,
                Preset _preset = Preset::NONE);
    void create(GLenum _target, GLenum _format, GLenum _iFormat,
                const string& _filePath,
                Preset _preset = Preset::NONE);

    void resize(glm::uvec3 _size);
    void buffer_memory(const void* _data, uint _z = 0);
    void buffer_file(const string& _filePath, uint _z = 0);
    void set_preset(Preset _preset);

    void bind(GLenum _slot = gl::NONE);
    void set_param(GLenum _name, GLenum _value);

    GLuint tex = 0;
    GLenum target;
    GLenum format;
    GLenum iFormat;
    glm::uvec3 size;

private:
    int channels;
    int dimensions;
    GLenum dataType;
};

}
