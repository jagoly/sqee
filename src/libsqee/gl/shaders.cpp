#include <iostream>

#include <gl/shaders.hpp>
#include <misc/files.hpp>

using namespace sq;

Shader::Shader() {
    prog = gl::CreateProgram();
}

Shader::~Shader() {
    gl::DeleteProgram(prog);
    gl::DeleteShader(vert);
    gl::DeleteShader(frag);
    gl::DeleteShader(geom);
}

bool Shader::load_from_file(const std::string& _fPath, GLenum _type) {
    std::string fPath = SQEE_SHD + _fPath + ".glsl";

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
}

void Shader::use() {
    gl::UseProgram(prog);
}

void Shader::add_uniform(const std::string& _name, UType _type) {
    uniforms[_name] = Uniform(gl::GetUniformLocation(prog, _name.c_str()), 1, _type);
}
void Shader::add_uniform_v(const std::string& _name, uint _cntgth, UType _type) {
    uniforms[_name] = Uniform(gl::GetUniformLocation(prog, _name.c_str()), _cntgth, _type);
}

bool Shader::set_uniform_f(const std::string& _name, const GLfloat _value) {
    if (uniforms[_name].type != UType::u_1f) {
        std::cout << "ERROR: Uniform \"" << _name << "\" is not a u_1f" << std::endl;
        return true;
    }
    gl::Uniform1f(uniforms[_name].ref, _value);
    return false;
}
bool Shader::set_uniform_u(const std::string& _name, const GLuint _value) {
    if (uniforms[_name].type != UType::u_1u) {
        std::cout << "ERROR: Uniform \"" << _name << "\" is not a u_1u" << std::endl;
        return true;
    }
    gl::Uniform1ui(uniforms[_name].ref, _value);
    return false;
}
bool Shader::set_uniform_i(const std::string& _name, const GLint _value) {
    if (uniforms[_name].type != UType::u_1i) {
        std::cout << "ERROR: Uniform \"" << _name << "\" is not a u_1i" << std::endl;
        return true;
    }
    gl::Uniform1i(uniforms[_name].ref, _value);
    return false;
}

bool Shader::set_uniform_fv(const std::string& _name, const GLfloat* _value) {
    GLint ref = uniforms[_name].ref;
    uint cnt = uniforms[_name].cnt;
    UType type = uniforms[_name].type;

    if      (type == UType::u_1f)
        gl::Uniform1fv(ref, cnt, _value);
    else if (type == UType::u_2f)
        gl::Uniform2fv(ref, cnt, _value);
    else if (type == UType::u_3f)
        gl::Uniform3fv(ref, cnt, _value);
    else if (type == UType::u_4f)
        gl::Uniform4fv(ref, cnt, _value);
    else {
        std::cout << "ERROR: Uniform \"" << _name << "\" is not a GLfloat(v)" << std::endl;
        return true;
    } return false;
}
bool Shader::set_uniform_uv(const std::string& _name, const GLuint* _value) {
    GLint ref = uniforms[_name].ref;
    uint cnt = uniforms[_name].cnt;
    UType type = uniforms[_name].type;

    if      (type == UType::u_1u)
        gl::Uniform1uiv(ref, cnt, _value);
    else if (type == UType::u_2u)
        gl::Uniform2uiv(ref, cnt, _value);
    else if (type == UType::u_3u)
        gl::Uniform3uiv(ref, cnt, _value);
    else if (type == UType::u_4u)
        gl::Uniform4uiv(ref, cnt, _value);
    else {
        std::cout << "ERROR: Uniform \"" << _name << "\" is not a GLuint(v)" << std::endl;
        return true;
    } return false;
}
bool Shader::set_uniform_iv(const std::string& _name, const GLint* _value) {
    GLint ref = uniforms[_name].ref;
    uint cnt = uniforms[_name].cnt;
    UType type = uniforms[_name].type;

    if      (type == UType::u_1i)
        gl::Uniform1iv(ref, cnt, _value);
    else if (type == UType::u_2i)
        gl::Uniform2iv(ref, cnt, _value);
    else if (type == UType::u_3i)
        gl::Uniform3iv(ref, cnt, _value);
    else if (type == UType::u_4i)
        gl::Uniform4iv(ref, cnt, _value);
    else {
        std::cout << "ERROR: Uniform \"" << _name << "\" is not a GLint(v)" << std::endl;
        return true;
    } return false;
}

bool Shader::set_uniform_mv(const std::string& _name, bool _transpose, const GLfloat* _value) {
    GLint ref = uniforms[_name].ref;
    uint cnt = uniforms[_name].cnt;
    UType type = uniforms[_name].type;

    if      (type == UType::u_2m)
        gl::UniformMatrix2fv(ref, cnt, _transpose, _value);
    else if (type == UType::u_23m)
        gl::UniformMatrix2x3fv(ref, cnt, _transpose, _value);
    else if (type == UType::u_24m)
        gl::UniformMatrix2x4fv(ref, cnt, _transpose, _value);
    else if (type == UType::u_3m)
        gl::UniformMatrix3fv(ref, cnt, _transpose, _value);
    else if (type == UType::u_32m)
        gl::UniformMatrix3x2fv(ref, cnt, _transpose, _value);
    else if (type == UType::u_34m)
        gl::UniformMatrix3x4fv(ref, cnt, _transpose, _value);
    else if (type == UType::u_4m)
        gl::UniformMatrix4fv(ref, cnt, _transpose, _value);
    else if (type == UType::u_42m)
        gl::UniformMatrix4x2fv(ref, cnt, _transpose, _value);
    else if (type == UType::u_43m)
        gl::UniformMatrix4x3fv(ref, cnt, _transpose, _value);
    else {
        std::cout << "ERROR: Uniform \"" << _name << "\" is not a matrix(v)" << std::endl;
        return true;
    } return false;
}
