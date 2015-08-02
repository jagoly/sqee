#pragma once
#include <sqee/forward.hpp>

namespace sq {

class Shader : NonCopyable {
friend class Pipeline;
public:
    Shader(GLenum _stage);
    ~Shader();

    void load(const string& _source);
    void load(const pair<string, string>& _srcPath);
    void add_uniform(const string& _name, uint _cnt = 1u);

    template <class T>
    void set_sca(const string& _name, const T& _value);
    template <class T>
    void set_scaptr(const string& _name, const T* _value);

    template <class T>
    void set_vec(const string& _name, const T& _value);
    template <class T, class D>
    void set_vecptr(const string& _name, const D* _value);

    template <class T>
    void set_mat(const string& _name, const T& _value, bool _transpose = false);
    template <class T, class D>
    void set_matptr(const string& _name, const D* _value, bool _transpose = false);

private:
    GLuint prog = 0u;
    GLenum stage;
    GLbitfield stageBit;

    struct Uniform {
        Uniform(uint _cnt) : cnt(_cnt) {}
        const uint cnt; GLint ref;
    };

    unordered_map<string, Uniform> uniforms;
};


class Pipeline : NonCopyable {
public:
    Pipeline();
    ~Pipeline();

    void use_shader(const Shader& _shader);
    void disable_stages(bool _vert, bool _geom, bool _frag);
    void bind();

private:
    GLuint pipeline = 0;
};

}
