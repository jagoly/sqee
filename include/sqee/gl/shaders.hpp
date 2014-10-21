#pragma once

#include <string>
#include <unordered_map>

#include <gl/gl.hpp>
#include <misc/containers.hpp>
// ^ need to make shaders follow conventions of other sqee resources

namespace sq {

enum class UType {
    f1, f2, f3, f4,
    u1, u2, u3, u4,
    i1, i2, i3, i4,
    m2, m23, m24,
    m3, m32, m34,
    m4, m42, m43
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

    void set_uniform_f(const std::string& _name, const GLfloat _value);
    void set_uniform_u(const std::string& _name, const GLuint _value);
    void set_uniform_i(const std::string& _name, const GLint _value);
    void set_uniform_fv(const std::string& _name, const GLfloat* _value);
    void set_uniform_uv(const std::string& _name, const GLuint* _value);
    void set_uniform_iv(const std::string& _name, const GLint* _value);
    void set_uniform_mv(const std::string& _name, bool _transpose, const GLfloat* _value);

private:
    std::unordered_map<std::string, Uniform> uniforms;
    GLuint vert, geom, frag;
};

}
