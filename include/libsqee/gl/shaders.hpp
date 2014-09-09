#pragma once

#include <string>
#include <map>

#include <gl/gl.hpp>

namespace sq {

enum class U_TYPE {
    u_1f, u_2f, u_3f, u_4f,
    u_1u, u_2u, u_3u, u_4u,
    u_1i, u_2i, u_3i, u_4i,
    u_2m, u_23m, u_24m,
    u_3m, u_32m, u_34m,
    u_4m, u_42m, u_43m
};

struct Uniform {
    Uniform() = default;
    Uniform(GLint _ref, uint _cnt, U_TYPE _type) {
        ref = _ref; cnt = _cnt; type = _type;
    }
    GLint ref; uint cnt; U_TYPE type;
};

struct Shader {
    Shader();
    ~Shader();

    GLuint prog;
    std::map<std::string, Uniform> uniforms;
    GLuint vert, geom, tess, frag;

    bool load_from_file(std::string _path, GLenum _type);
    void build();
    void use();

    void add_uniform(std::string _name, U_TYPE _type);
    void add_uniform_v(std::string _name, uint _length, U_TYPE _type);

    bool set_uniform_f(std::string _name, const GLfloat   _value);
    bool set_uniform_u(std::string _name, const GLuint    _value);
    bool set_uniform_i(std::string _name, const GLint     _value);
    bool set_uniform_fv(std::string _name, const GLfloat* _value);
    bool set_uniform_uv(std::string _name, const GLuint*  _value);
    bool set_uniform_iv(std::string _name, const GLint*   _value);
    bool set_uniform_mv(std::string _name, bool _transpose, const GLfloat*  _value);
};

}
