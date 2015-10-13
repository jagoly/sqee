#include <glm/gtc/type_ptr.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/gl/Shaders.hpp>

using namespace sq;

Shader::Shader(GLenum _stage) : stage(_stage) {
    if (stage == gl::VERTEX_SHADER) stageBit = gl::VERTEX_SHADER_BIT;
    else if (stage == gl::FRAGMENT_SHADER) stageBit = gl::FRAGMENT_SHADER_BIT;
    else if (stage == gl::GEOMETRY_SHADER) stageBit = gl::GEOMETRY_SHADER_BIT;
}

Shader::~Shader() {
    gl::DeleteProgram(program);
}

void Shader::add_uniform(const string& _name, uint _cnt) {
    uniforms.emplace(_name, Uniform(_cnt));
}

void Shader::load(const string& _source) {
    const char* src = _source.c_str();
    if (program != 0u) gl::DeleteProgram(program);
    program = gl::CreateShaderProgramv(stage, 1, &src);
    int length = 0; char log[2048]; gl::GetProgramInfoLog(program, 2048, &length, log);
    if (length > 0) { string logStr(log); logStr.erase(logStr.rfind('\n'));
        log_error("Failed to compile shader from string\n%s", logStr); }
    for (auto& uf : uniforms) uf.second.ref = gl::GetUniformLocation(program, uf.first.c_str());
}

void Shader::load(const string& _source, const string& _path) {
    const char* src = _source.c_str();
    if (program != 0u) gl::DeleteProgram(program);
    program = gl::CreateShaderProgramv(stage, 1, &src);
    int length = 0; char log[2048]; gl::GetProgramInfoLog(program, 2048, &length, log);
    if (length > 0) { string logStr(log); logStr.erase(logStr.rfind('\n'));
        log_error("Failed to compile shader from %s\n%s", _path, logStr); }
    for (auto& uf : uniforms) uf.second.ref = gl::GetUniformLocation(program, uf.first.c_str());
}


// Uniforms /////

template <class T> struct Trait;

template <class T>
void Shader::set_sca(const string& _name, const T& _value) const {
    Trait<T>::func(program, uniforms.at(_name).ref, 1, &_value);
}
template <class T>
void Shader::set_scaptr(const string& _name, const T* _value) const {
    const Uniform& uform = uniforms.at(_name);
    Trait<T>::func(program, uform.ref, uform.cnt, _value);
}

template <class T>
void Shader::set_vec(const string& _name, const T& _value) const {
    const Uniform& uform = uniforms.at(_name);
    Trait<T>::func(program, uform.ref, uform.cnt, glm::value_ptr(_value));
}
template <class T, class D>
void Shader::set_vecptr(const string& _name, const D* _value) const {
    const Uniform& uform = uniforms.at(_name);
    Trait<T>::func(program, uform.ref, uform.cnt, _value);
}

template <class T>
void Shader::set_mat(const string& _name, const T& _value) const {
    const Uniform& uform = uniforms.at(_name);
    Trait<T>::func(program, uform.ref, uform.cnt, false, glm::value_ptr(_value));
}
template <class T, class D>
void Shader::set_matptr(const string& _name, const D* _value) const {
    const Uniform& uform = uniforms.at(_name);
    Trait<T>::func(program, uform.ref, uform.cnt, false, _value);
}

#define UFORM_DEFINE(type, f) \
template<> struct Trait<type> { \
    static constexpr void (*func)(GLuint, GLint, GLsizei, const type*) = &f; \
}; \
template void Shader::set_sca<type>(const string& _name, const type& _value) const; \
template void Shader::set_scaptr<type>(const string& _name, const type* _value) const;

UFORM_DEFINE(GLfloat, gl::ProgramUniform1fv)
UFORM_DEFINE(GLint,   gl::ProgramUniform1iv)
UFORM_DEFINE(GLuint,  gl::ProgramUniform1uiv)
#undef UFORM_DEFINE

#define UFORM_DEFINE(glmtype, datatype, f) \
template<> struct Trait<glmtype> { \
    static constexpr void (*func)(GLuint, GLint, GLsizei, const datatype*) = &f; \
}; \
template void Shader::set_vec<glmtype>(const string& _name, const glmtype& _value) const; \
template void Shader::set_vecptr<glmtype>(const string& _name, const datatype* _value) const;

UFORM_DEFINE(fvec2, GLfloat, gl::ProgramUniform2fv)
UFORM_DEFINE(fvec3, GLfloat, gl::ProgramUniform3fv)
UFORM_DEFINE(fvec4, GLfloat, gl::ProgramUniform4fv)
UFORM_DEFINE(ivec2, GLint,   gl::ProgramUniform2iv)
UFORM_DEFINE(ivec3, GLint,   gl::ProgramUniform3iv)
UFORM_DEFINE(ivec4, GLint,   gl::ProgramUniform4iv)
UFORM_DEFINE(uvec2, GLuint,  gl::ProgramUniform2uiv)
UFORM_DEFINE(uvec3, GLuint,  gl::ProgramUniform3uiv)
UFORM_DEFINE(uvec4, GLuint,  gl::ProgramUniform4uiv)
#undef UFORM_DEFINE

#define UFORM_DEFINE(glmtype, f) \
template<> struct Trait<glmtype> { \
    static constexpr void (*func)(GLuint, GLint, GLsizei, GLboolean, const float*) = &f; \
}; \
template void Shader::set_mat<glmtype>(const string& _name, const glmtype& _value) const; \
template void Shader::set_matptr<glmtype>(const string& _name, const GLfloat* _value) const;

UFORM_DEFINE(fmat2, gl::ProgramUniformMatrix2fv)
UFORM_DEFINE(fmat3, gl::ProgramUniformMatrix3fv)
UFORM_DEFINE(fmat4, gl::ProgramUniformMatrix4fv)
#undef UFORM_DEFINE


Pipeline::Pipeline() {
    gl::CreateProgramPipelines(1, &pipeline);
}

Pipeline::~Pipeline() {
    gl::DeleteProgramPipelines(1, &pipeline);
}

void Pipeline::use_shader(const Shader& _shader) const {
    gl::UseProgramStages(pipeline, _shader.stageBit, _shader.program);
}

void Pipeline::disable_stages(bool _vert, bool _geom, bool _frag) const {
    gl::UseProgramStages(pipeline, gl::FRAGMENT_SHADER_BIT * _frag |
                                   gl::GEOMETRY_SHADER_BIT * _geom |
                                   gl::VERTEX_SHADER_BIT * _vert, 0);
}

void Pipeline::bind() const {
    gl::BindProgramPipeline(pipeline);
}
