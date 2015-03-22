#pragma once
#include "forward.hpp"

#include "misc/resholder.hpp"

namespace sq {

class Texture : NonCopyable {
public:
    enum class Preset { NONE, N_C, N_R, N_M, L_C, L_R, L_M, M_C, M_R, M_M };

    void create(GLenum _target, GLenum _format, GLenum _iFormat, uint _channels, Preset _preset);
    ~Texture();

    void resize(uvec3 _size);
    void buffer_memory(const void* _data, uint _z);
    void buffer_file(const string& _filePath, uint _z);
    void set_param(GLenum _name, GLenum _value);
    void set_preset(Preset _preset);
    void gen_mipmap();

    void bind(GLenum _slot);
    void bind();

    GLuint tex = 0;
    GLenum target;
    GLenum format;
    GLenum iFormat;
    GLenum dataType;
    uvec3 size;

private:
    uint channels;
    uint dimensions;
};

namespace res { ResHolder<Texture>& texture(); }

}
