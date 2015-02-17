#include "app/logging.hpp"
#include "gl/gl_ext_3_3.hpp"
#include "gl/maths.hpp"
#include "misc/files.hpp"

#include "gl/shaders.hpp"

using namespace sq;

Shader::~Shader() {
    gl::DeleteProgram(prog);
}

void Shader::load(const string& _fPath, GLenum _stage) {
    uniforms.clear();

    string fPath = "res/shaders/" + _fPath + ".glsl";

    if (_stage == gl::VERTEX_SHADER)
        stages = gl::VERTEX_SHADER_BIT;
    else if (_stage == gl::FRAGMENT_SHADER)
        stages = gl::FRAGMENT_SHADER_BIT;
    else log_error("Invalid shader stage when loading $0", fPath);

    string str = sq::get_string_from_file(fPath);
    if (str.empty()) log_error("Shader file $0 not found", fPath);

    const char* src = str.c_str();
    prog = gl::CreateShaderProgramv(_stage, 1, &src);
    int length = 0; char log[2048];
    gl::GetProgramInfoLog(prog, 2048, &length, log);
    if (length > 0) log_error("Failed to compile shader from $0$L$1$L", fPath, log);
}

void Shader::add_uniform(const string& _name, uint _cnt) {
    GLint ref = gl::GetUniformLocation(prog, _name.c_str());
    uniforms.emplace(_name, Uniform(ref, _cnt));
}


/// Uniforms ///

template <class T> struct Trait;

template <class T>
void Shader::set_sca(const string& _name, const T& _value) {
    Trait<T>::func(prog, uniforms.at(_name).ref, 1, &_value);
}
template <class T>
void Shader::set_ptr(const string& _name, const T* _value) {
    const Uniform& uform = uniforms.at(_name);
    Trait<T>::func(prog, uform.ref, uform.cnt, _value);
}

template <class T>
void Shader::set_glmvec(const string& _name, const T& _value) {
    const Uniform& uform = uniforms.at(_name);
    Trait<T>::func(prog, uform.ref, uform.cnt, glm::value_ptr(_value));
}
template <class T, class D>
void Shader::set_ptrvec(const string& _name, const D* _value) {
    const Uniform& uform = uniforms.at(_name);
    Trait<T>::func(prog, uform.ref, uform.cnt, _value);
}

template <class T>
void Shader::set_glmmat(const string& _name, const T& _value, bool _transpose) {
    const Uniform& uform = uniforms.at(_name);
    Trait<T>::func(prog, uform.ref, uform.cnt, _transpose, glm::value_ptr(_value));
}
template <class T, class D>
void Shader::set_ptrmat(const string& _name, const D* _value, bool _transpose) {
    const Uniform& uform = uniforms.at(_name);
    Trait<T>::func(prog, uform.ref, uform.cnt, _transpose, _value);
}

#define UFORM_DEFINE(type, f) \
template<> struct Trait<type> { \
    static constexpr void (*func)(GLuint, GLint, GLsizei, const type*) = &f; \
}; \
template void Shader::set_sca<type>(const string& _name, const type& _value); \
template void Shader::set_ptr<type>(const string& _name, const type* _value);

UFORM_DEFINE(GLfloat, gl::ProgramUniform1fv)
UFORM_DEFINE(GLint,   gl::ProgramUniform1iv)
UFORM_DEFINE(GLuint,  gl::ProgramUniform1uiv)
#undef UFORM_DEFINE

#define UFORM_DEFINE(glmtype, datatype, f) \
template<> struct Trait<glmtype> { \
    static constexpr void (*func)(GLuint, GLint, GLsizei, const datatype*) = &f; \
}; \
template void Shader::set_glmvec<glmtype>(const string& _name, const glmtype& _value); \
template void Shader::set_ptrvec<glmtype>(const string& _name, const datatype* _value);

UFORM_DEFINE(glm::vec2,  GLfloat, gl::ProgramUniform2fv)
UFORM_DEFINE(glm::vec3,  GLfloat, gl::ProgramUniform3fv)
UFORM_DEFINE(glm::vec4,  GLfloat, gl::ProgramUniform4fv)
UFORM_DEFINE(glm::ivec2, GLint,   gl::ProgramUniform2iv)
UFORM_DEFINE(glm::ivec3, GLint,   gl::ProgramUniform3iv)
UFORM_DEFINE(glm::ivec4, GLint,   gl::ProgramUniform4iv)
UFORM_DEFINE(glm::uvec2, GLuint,  gl::ProgramUniform2uiv)
UFORM_DEFINE(glm::uvec3, GLuint,  gl::ProgramUniform3uiv)
UFORM_DEFINE(glm::uvec4, GLuint,  gl::ProgramUniform4uiv)
#undef UFORM_DEFINE

#define UFORM_DEFINE(glmtype, f) \
template<> struct Trait<glmtype> { \
    static constexpr void (*func)(GLuint, GLint, GLsizei, GLboolean, const float*) = &f; \
}; \
template void Shader::set_glmmat<glmtype>(const string& _name, const glmtype& _value, bool _transpose); \
template void Shader::set_ptrmat<glmtype>(const string& _name, const GLfloat* _value, bool _transpose);

UFORM_DEFINE(glm::mat2,   gl::ProgramUniformMatrix2fv)
UFORM_DEFINE(glm::mat2x3, gl::ProgramUniformMatrix2x3fv)
UFORM_DEFINE(glm::mat2x4, gl::ProgramUniformMatrix2x4fv)
UFORM_DEFINE(glm::mat3,   gl::ProgramUniformMatrix3fv)
UFORM_DEFINE(glm::mat3x2, gl::ProgramUniformMatrix3x2fv)
UFORM_DEFINE(glm::mat3x4, gl::ProgramUniformMatrix3x4fv)
UFORM_DEFINE(glm::mat4,   gl::ProgramUniformMatrix4fv)
UFORM_DEFINE(glm::mat4x2, gl::ProgramUniformMatrix4x2fv)
UFORM_DEFINE(glm::mat4x3, gl::ProgramUniformMatrix4x3fv)
#undef UFORM_DEFINE


Pipeline::Pipeline() {
    gl::GenProgramPipelines(1, &pipeline);
}

Pipeline::~Pipeline() {
    gl::DeleteProgramPipelines(1, &pipeline);
}

void Pipeline::use_shader(const Shader& _shader) {
    gl::UseProgramStages(pipeline, _shader.stages, _shader.prog);
}

void Pipeline::disable_stages(GLbitfield _stages) {
    gl::UseProgramStages(pipeline, _stages, 0);
}

void Pipeline::bind() {
    gl::BindProgramPipeline(pipeline);
}
