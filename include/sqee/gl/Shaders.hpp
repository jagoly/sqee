#pragma once
#include <sqee/forward.hpp>
#include <unordered_map>

namespace sq {

/// A class for single stage OpenGL shader program
class Shader final : NonCopyable {
friend class Pipeline;
public:
    Shader(GLenum _stage);
    ~Shader();

    /// Add a uniform to the shader. Call before load().
    void add_uniform(const string& _name, uint _cnt = 1u);

    /// Load a shader from a string
    void load(const string& _source);

    /// Load a shader from a string and set a path for debug
    void load(const string& _source, const string& _path);

    template <class T>
    /// Set a scalar uniform from a reference
    void set_sca(const string& _name, const T& _value) const;

    template <class T>
    /// Set a scalar uniform from a pointer
    void set_scaptr(const string& _name, const T* _value) const;

    template <class T>
    /// Set a vector uniform from a reference
    void set_vec(const string& _name, const T& _value) const;

    template <class T, class D>
    /// Set a vector uniform from a pointer
    void set_vecptr(const string& _name, const D* _value) const;

    template <class T>
    /// Set a matrix uniform from a reference
    void set_mat(const string& _name, const T& _value) const;

    template <class T, class D>
    /// Set a matrix uniform from a pointer
    void set_matptr(const string& _name, const D* _value) const;

    /// The OpenGL handle
    GLuint program = 0u;

private:
    const GLenum stage;
    GLbitfield stageBit;

    struct Uniform {
        Uniform(uint _cnt) : cnt(_cnt) {}
        const uint cnt; GLint ref;
    };

    std::unordered_map<string, Uniform> uniforms;
};


/// A class for OpenGL Program Pipeline Objects
class Pipeline final : NonCopyable {
public:
    Pipeline();
    ~Pipeline();

    /// Attach a shader stage to the pipeline
    void use_shader(const Shader& _shader) const;

    /// Disable one or more shader stages
    void disable_stages(bool _vert, bool _geom, bool _frag) const;

    /// Bind the pipeline object
    void bind() const;

    /// The OpenGL handle
    GLuint pipeline = 0u;
};

}
