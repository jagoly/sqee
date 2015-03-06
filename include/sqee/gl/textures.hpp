#pragma once
#include "forward.hpp"

#include "misc/resholder.hpp"

namespace sq {

class Texture : NonCopyable {
public:
    enum class Preset { NONE, N_C, N_R, L_C, L_R, M_C, M_R };

    void create(GLenum _target, GLenum _format, GLenum _iFormat,
                Preset _preset, const string& _path = "");
    ~Texture();

    void resize(glm::uvec3 _size);
    void buffer_memory(const void* _data, uint _z = 0);
    void buffer_file(const string& _filePath, uint _z = 0);
    void set_preset(Preset _preset);

    void bind(GLenum _slot = 0);
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

namespace res { ResHolder<Texture>& texture(); }

}
