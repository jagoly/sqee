#pragma once

#include <unordered_map>
#include <iterator>

#include <gl/gl.hpp>
#include <misc/containers.hpp>
// ^ need to make shaders follow conventions of other sqee resources

namespace sq {

class Shader {
public:
    Shader();
    ~Shader();

    bool load_from_file(const string& _path, GLenum _type);
    void build();
    void use();

    void add_uniform(const string& _name, uint _cnt = 1);

    void set_F(const string& _name, const GLfloat _value);
    void set_I(const string& _name, const GLint _value);
    void set_U(const string& _name, const GLuint _value);

    template <class T>
    void set_glmvec(const string& _name, const T& _value);
    template <class T>
    void set_glmmat(const string& _name, const T& _value);

    template <class T, typename D>
    void set_ptrvec(const string& _name, const D* _value);
    template <class T, class D>
    void set_ptrmat(const string& _name, const D* _value);

    GLuint prog;

private:
    struct Uniform {
        Uniform(GLint _ref, uint _cnt)
            : ref(_ref), cnt(_cnt) {}
        const GLint ref; const uint cnt;
    };

    std::unordered_map<string, Uniform> uniforms;
    GLuint vert, geom, frag;
};

}
