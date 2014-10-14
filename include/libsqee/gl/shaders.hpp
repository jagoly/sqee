#pragma once

#include <string>
#include <unordered_map>

#include <gl/gl.hpp>
#include <misc/containers.hpp>
// ^ need to make shaders follow conventions of other sqee resources

#ifndef SQEE_SHD
#define SQEE_SHD "res/shaders/"
#endif

namespace sq {

enum class UType {
    u_1f, u_2f, u_3f, u_4f,
    u_1u, u_2u, u_3u, u_4u,
    u_1i, u_2i, u_3i, u_4i,
    u_2m, u_23m, u_24m,
    u_3m, u_32m, u_34m,
    u_4m, u_42m, u_43m
};

class Shader {
public:
    struct Uniform {
        Uniform() = default;
        Uniform(GLint _ref, uint _cnt, UType _type) {
            ref = _ref; cnt = _cnt; type = _type;
        }
        GLint ref; uint cnt; UType type;
    };

    Shader();
    ~Shader();

    GLuint prog;

    bool load_from_file(const std::string& _path, GLenum _type);
    void build();
    void use();

    void add_uniform(const std::string& _name, UType _type);
    void add_uniform_v(const std::string& _name, uint _length, UType _type);

    bool set_uniform_f(const std::string& _name, const GLfloat _value);
    bool set_uniform_u(const std::string& _name, const GLuint _value);
    bool set_uniform_i(const std::string& _name, const GLint _value);
    bool set_uniform_fv(const std::string& _name, const GLfloat* _value);
    bool set_uniform_uv(const std::string& _name, const GLuint* _value);
    bool set_uniform_iv(const std::string& _name, const GLint* _value);
    bool set_uniform_mv(const std::string& _name, bool _transpose, const GLfloat* _value);

private:
    std::unordered_map<std::string, Uniform> uniforms;
    GLuint vert, geom, frag;
};

}
