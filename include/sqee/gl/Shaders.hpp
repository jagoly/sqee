#pragma once
#include <sqee/forward.hpp>
#include <unordered_map>

namespace sq {

/// A class for single stage OpenGL shader program
class Shader : NonCopyable {
friend class Pipeline;
public:
    Shader(GLenum _stage);
    ~Shader();

    /// Add a uniform to the shader. Must be called BEFORE load().
    void add_uniform(const string& _name, uint _cnt = 1u);

    /// Load a shader from a string
    void load(const string& _source);

    /// Load a shader from a string and set a path for debug
    void load(const pair<string, string>& _srcPath);

    /// Set all subroutine uniforms from a pointer
    void set_subroutines(uint _count, const uint* _values);

    template <class T>
    /// Set a scalar uniform from a reference
    void set_sca(const string& _name, const T& _value);

    template <class T>
    /// Set a scalar uniform from a pointer
    void set_scaptr(const string& _name, const T* _value);

    template <class T>
    /// Set a vector uniform from a reference
    void set_vec(const string& _name, const T& _value);

    template <class T, class D>
    /// Set a vector uniform from a pointer
    void set_vecptr(const string& _name, const D* _value);

    template <class T>
    /// Set a matrix uniform from a reference
    void set_mat(const string& _name, const T& _value);

    template <class T, class D>
    /// Set a matrix uniform from a pointer
    void set_matptr(const string& _name, const D* _value);

    /// The OpenGL handle
    GLuint program = 0u;

protected:
    GLenum stage;
    GLbitfield stageBit;

    struct Uniform {
        Uniform(uint _cnt) : cnt(_cnt) {}
        const uint cnt; GLint ref;
    };

    std::unordered_map<string, Uniform> uniforms;
    std::unordered_map<string, GLint> subroutines;
};


/// A class for OpenGL Program Pipeline Objects
class Pipeline : NonCopyable {
public:
    Pipeline();
    ~Pipeline();

    /// Attach a shader stage to the pipeline
    void use_shader(const Shader& _shader);

    /// Disable one or more shader stages
    void disable_stages(bool _vert, bool _geom, bool _frag);

    /// Bind the pipeline object
    void bind();

    /// The OpenGL handle
    GLuint pipeline = 0u;
};

}
