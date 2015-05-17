#pragma once
#include <sqee/forward.hpp>

#include <sqee/misc/resholder.hpp>

namespace sq {

class Texture : NonCopyable {
public:
    virtual ~Texture();
    void create_base();

    using Preset = vector<pair<GLenum, GLenum>>;
    void set_param(GLenum _name, GLenum _value);
    void set_preset(Preset _preset);

    void bind(GLenum _slot);
    void bind();

    GLuint tex = 0;
    GLenum target;
    GLenum dataType;
    GLenum format;
    GLenum iFormat;
    uint channels;
};

class Texture2D : public Texture {
public:
    void create(GLenum _format, GLenum _iFormat, uint _channels);
    void resize(uvec2 _size);
    void buffer_memory(const void* _data);
    void buffer_file(const string& _filePath);
    void gen_mipmap();
    void viewport();
    uvec2 size;

    static const Preset& N_C();
    static const Preset& N_R();
    static const Preset& N_M();
    static const Preset& L_C();
    static const Preset& L_R();
    static const Preset& L_M();
    static const Preset& M_C();
    static const Preset& M_R();
    static const Preset& M_M();
};


class Texture2DArray : public Texture {
public:
    void create(GLenum _format, GLenum _iFormat, uint _channels);
    void resize(uvec3 _size);
    void buffer_memory(const void* _data, uint _z);
    void buffer_file(const string& _filePath, uint _z);
    void viewport();
    uvec3 size;

    static const Preset& N_C();
    static const Preset& N_R();
    static const Preset& N_M();
    static const Preset& L_C();
    static const Preset& L_R();
    static const Preset& L_M();
};


class TextureCube : public Texture {
public:
    void create(GLenum _format, GLenum _iFormat, uint _channels);
    void resize(uint _size);
    void buffer_memory(const void* _data, uint _face);
    void buffer_file(const string& _filePath, uint _face);
    void buffer_full(const string& _filePath, uint _size);
    void viewport();
    uint size;

    static const Preset& N_C();
    static const Preset& L_C();
};


namespace res {
ResHolder<Texture2D>& tex2D();
string& tex2D_path();

ResHolder<TextureCube>& texCube();
string& texCube_path();
}

}
