#include <iostream>

#include <gl/shaders.hpp>
#include <misc/files.hpp>

using namespace sq;

Shader::Shader() {
    prog = gl::CreateProgram();
}

Shader::~Shader() {
    gl::DeleteProgram(prog);
}

bool Shader::load_from_file(const std::string& _fPath, GLenum _type) {
    std::string fPath = SQ_SHADERS + _fPath + ".glsl";

    GLuint* shad;
    if      (_type == gl::VERTEX_SHADER)   shad = &vert;
    else if (_type == gl::GEOMETRY_SHADER) shad = &geom;
    else if (_type == gl::FRAGMENT_SHADER) shad = &frag;
    else {
        std::cout << "ERROR: Invalid shader type when loading \"" << fPath << "\"" << std::endl;
        return true;
    }
    *shad = gl::CreateShader(_type);

    std::string str = sq::get_string_from_file(fPath);
    if (str.empty()) {
        std::cout << "ERROR: Shader file \"" << fPath << "\" not found" << std::endl;
        return true;
    }

    const char* src = str.c_str();
    int cnt = str.size();
    gl::ShaderSource(*shad, 1, &src, &cnt);

    gl::CompileShader(*shad);
    int length = 0;
    char log[2048];
    gl::GetShaderInfoLog(*shad, 2048, &length, log);
    if (length) {
        std::cout << "ERROR: Failed to compile shader from \"" << fPath << "\"\n"
                  << "-------------------------\n"
                  << log << "-------------------------" << std::endl;
        return true;
    }
    gl::AttachShader(prog, *shad);

    return false;
}

void Shader::build() {
    gl::LinkProgram(prog);
    int length = 0;
    char log[2048];
    gl::GetProgramInfoLog(prog, 2048, &length, log);
    if (length) {
        std::cout << "ERROR: Failed to link shader\n"
                  << "-------------------------\n"
                  << log << "-------------------------" << std::endl;
    }

    gl::DeleteShader(vert);
    gl::DeleteShader(frag);
    gl::DeleteShader(geom);
}

void Shader::use() {
    gl::UseProgram(prog);
}

void Shader::add_uniform(const std::string& _name, UType _type) {
    uniforms.insert({_name, {gl::GetUniformLocation(prog, _name.c_str()), 1, _type}});
}
void Shader::add_uniform_v(const std::string& _name, uint _cnt, UType _type) {
    uniforms.insert({_name, {gl::GetUniformLocation(prog, _name.c_str()), _cnt, _type}});
}


void Shader::set_uniform_f(const std::string& _name, const GLfloat _value) {
    if (uniforms.at(_name).type != UType::f1)
        std::cout << "ERROR: Uniform \"" << _name << "\" is not a u_1f" << std::endl;
    gl::Uniform1f(uniforms.at(_name).ref, _value);
}
void Shader::set_uniform_u(const std::string& _name, const GLuint _value) {
    if (uniforms.at(_name).type != UType::u1)
        std::cout << "ERROR: Uniform \"" << _name << "\" is not a u_1u" << std::endl;
    gl::Uniform1ui(uniforms.at(_name).ref, _value);
}
void Shader::set_uniform_i(const std::string& _name, const GLint _value) {
    if (uniforms.at(_name).type != UType::i1)
        std::cout << "ERROR: Uniform \"" << _name << "\" is not a u_1i" << std::endl;
    gl::Uniform1i(uniforms.at(_name).ref, _value);
}

void Shader::set_uniform_fv(const std::string& _name, const GLfloat* _value) {
    GLint ref = uniforms.at(_name).ref;
    uint cnt = uniforms.at(_name).cnt;
    UType type = uniforms.at(_name).type;

    if      (type == UType::f1)
        gl::Uniform1fv(ref, cnt, _value);
    else if (type == UType::f2)
        gl::Uniform2fv(ref, cnt, _value);
    else if (type == UType::f3)
        gl::Uniform3fv(ref, cnt, _value);
    else if (type == UType::f4)
        gl::Uniform4fv(ref, cnt, _value);
    else std::cout << "ERROR: Uniform \"" << _name << "\" is not a GLfloat(v)" << std::endl;
}
void Shader::set_uniform_uv(const std::string& _name, const GLuint* _value) {
    GLint ref = uniforms.at(_name).ref;
    uint cnt = uniforms.at(_name).cnt;
    UType type = uniforms.at(_name).type;

    if      (type == UType::u1)
        gl::Uniform1uiv(ref, cnt, _value);
    else if (type == UType::u2)
        gl::Uniform2uiv(ref, cnt, _value);
    else if (type == UType::u3)
        gl::Uniform3uiv(ref, cnt, _value);
    else if (type == UType::u4)
        gl::Uniform4uiv(ref, cnt, _value);
    else std::cout << "ERROR: Uniform \"" << _name << "\" is not a GLuint(v)" << std::endl;
}
void Shader::set_uniform_iv(const std::string& _name, const GLint* _value) {
    GLint ref = uniforms.at(_name).ref;
    uint cnt = uniforms.at(_name).cnt;
    UType type = uniforms.at(_name).type;

    if      (type == UType::i1)
        gl::Uniform1iv(ref, cnt, _value);
    else if (type == UType::i2)
        gl::Uniform2iv(ref, cnt, _value);
    else if (type == UType::i3)
        gl::Uniform3iv(ref, cnt, _value);
    else if (type == UType::i4)
        gl::Uniform4iv(ref, cnt, _value);
    else std::cout << "ERROR: Uniform \"" << _name << "\" is not a GLint(v)" << std::endl;
}

void Shader::set_uniform_mv(const std::string& _name, bool _transpose, const GLfloat* _value) {
    GLint ref = uniforms.at(_name).ref;
    uint cnt = uniforms.at(_name).cnt;
    UType type = uniforms.at(_name).type;

    if      (type == UType::m2)
        gl::UniformMatrix2fv(ref, cnt, _transpose, _value);
    else if (type == UType::m23)
        gl::UniformMatrix2x3fv(ref, cnt, _transpose, _value);
    else if (type == UType::m24)
        gl::UniformMatrix2x4fv(ref, cnt, _transpose, _value);
    else if (type == UType::m3)
        gl::UniformMatrix3fv(ref, cnt, _transpose, _value);
    else if (type == UType::m32)
        gl::UniformMatrix3x2fv(ref, cnt, _transpose, _value);
    else if (type == UType::m34)
        gl::UniformMatrix3x4fv(ref, cnt, _transpose, _value);
    else if (type == UType::m4)
        gl::UniformMatrix4fv(ref, cnt, _transpose, _value);
    else if (type == UType::m42)
        gl::UniformMatrix4x2fv(ref, cnt, _transpose, _value);
    else if (type == UType::m43)
        gl::UniformMatrix4x3fv(ref, cnt, _transpose, _value);
    else std::cout << "ERROR: Uniform \"" << _name << "\" is not a matrix(v)" << std::endl;
}
