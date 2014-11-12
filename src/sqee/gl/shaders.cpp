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

bool Shader::load_from_file(const string& _fPath, GLenum _type) {
    string fPath = SQ_SHADERS + _fPath + ".glsl";

    GLuint* shad;
    if      (_type == gl::VERTEX_SHADER)   shad = &vert;
    else if (_type == gl::GEOMETRY_SHADER) shad = &geom;
    else if (_type == gl::FRAGMENT_SHADER) shad = &frag;
    else {
        std::cout << "ERROR: Invalid shader type when loading \"" << fPath << "\"" << std::endl;
        return true;
    }
    *shad = gl::CreateShader(_type);

    string str = sq::get_string_from_file(fPath);
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

void Shader::add_uniform(const string& _name, uint _cnt) {
    uniforms.emplace(_name, Uniform(gl::GetUniformLocation(prog, _name.c_str()), _cnt));
}


void Shader::set_F(const string& _name, GLfloat _value) {
    gl::Uniform1f(uniforms.at(_name).ref, _value);
}
void Shader::set_I(const string& _name, GLint _value) {
    gl::Uniform1i(uniforms.at(_name).ref, _value);
}
void Shader::set_U(const string& _name, GLuint _value) {
    gl::Uniform1ui(uniforms.at(_name).ref, _value);
}


namespace sq {

template <class T> struct UFunc;

template <class T>
void Shader::set_glmvec(const string& _name, const T& _value) {
    const Uniform& uform = uniforms.at(_name);
    UFunc<T>::f(uform.ref, uform.cnt, glm::value_ptr(_value));
}
template <class T>
void Shader::set_glmmat(const string& _name, const T& _value) {
    const Uniform& uform = uniforms.at(_name);
    UFunc<T>::f(uform.ref, uform.cnt, false, glm::value_ptr(_value));
}

template <class T, class D>
void Shader::set_ptrvec(const string& _name, const D* _value) {
    const Uniform& uform = uniforms.at(_name);
    UFunc<T>::f(uform.ref, uform.cnt, _value);
}
template <class T, class D>
void Shader::set_ptrmat(const string& _name, const D* _value) {
    const Uniform& uform = uniforms.at(_name);
    UFunc<T>::f(uform.ref, uform.cnt, false, _value);
}


#define UFORM_DEFINE(glmtype, datatype, func) \
template<> struct UFunc<glmtype> { \
    static constexpr void (*f)(GLint, GLsizei, const datatype*) = &func; \
}; \
template void Shader::set_glmvec<glmtype>(const string& _name, const glmtype& _value); \
template void Shader::set_ptrvec<glmtype>(const string& _name, const datatype* _value);

UFORM_DEFINE(glm::vec2,  GLfloat, gl::Uniform2fv)
UFORM_DEFINE(glm::vec3,  GLfloat, gl::Uniform3fv)
UFORM_DEFINE(glm::vec4,  GLfloat, gl::Uniform4fv)
UFORM_DEFINE(glm::ivec2, GLint,   gl::Uniform2iv)
UFORM_DEFINE(glm::ivec3, GLint,   gl::Uniform3iv)
UFORM_DEFINE(glm::ivec4, GLint,   gl::Uniform4iv)
UFORM_DEFINE(glm::uvec2, GLuint,  gl::Uniform2uiv)
UFORM_DEFINE(glm::uvec3, GLuint,  gl::Uniform3uiv)
UFORM_DEFINE(glm::uvec4, GLuint,  gl::Uniform4uiv)
#undef UFORM_DEFINE


#define UFORM_DEFINE(glmtype, func) \
template<> struct UFunc<glmtype> { \
    static constexpr void (*f)(GLint, GLsizei, GLboolean, const GLfloat*) = &func; \
}; \
template void Shader::set_glmmat<glmtype>(const string& _name, const glmtype& _value); \
template void Shader::set_ptrmat<glmtype>(const string& _name, const GLfloat* _value);

UFORM_DEFINE(glm::mat2,   gl::UniformMatrix2fv)
UFORM_DEFINE(glm::mat2x3, gl::UniformMatrix2x3fv)
UFORM_DEFINE(glm::mat2x4, gl::UniformMatrix2x4fv)
UFORM_DEFINE(glm::mat3,   gl::UniformMatrix3fv)
UFORM_DEFINE(glm::mat3x2, gl::UniformMatrix3x2fv)
UFORM_DEFINE(glm::mat3x4, gl::UniformMatrix3x4fv)
UFORM_DEFINE(glm::mat4,   gl::UniformMatrix4fv)
UFORM_DEFINE(glm::mat4x2, gl::UniformMatrix4x2fv)
UFORM_DEFINE(glm::mat4x3, gl::UniformMatrix4x3fv)
#undef UFORM_DEFINE

}
