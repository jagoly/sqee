#pragma once
#include <sqee/forward.hpp>

#include <unordered_map>

namespace sq {

class Shader : NonCopyable {
friend class Pipeline;
public:
    Shader(GLenum _stage);
    ~Shader();

    void load(const string& _shaderStr);
    void add_uniform(const string& _name, uint _cnt = 1);

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
    GLuint prog = 0;
    GLenum stage;
    GLbitfield stageBit;

    struct Uniform {
        Uniform(GLint _ref, uint _cnt) : ref(_ref), cnt(_cnt) {}
        const GLint ref; const uint cnt;
    };
    std::unordered_map<string, Uniform> uniforms;
};


class Pipeline : NonCopyable {
public:
    Pipeline();
    ~Pipeline();

    void use_shader(const Shader& _shader);
    void disable_stages(GLbitfield _stageBits);
    void bind();

private:
    GLuint pipeline = 0;
};

}
