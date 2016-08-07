#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/gl/Shaders.hpp>
#include <sqee/assert.hpp>

using namespace sq;

Shader::Shader(GLenum _stage) : stage(_stage) {
    if (stage == gl::VERTEX_SHADER) stageBit = gl::VERTEX_SHADER_BIT;
    else if (stage == gl::FRAGMENT_SHADER) stageBit = gl::FRAGMENT_SHADER_BIT;
    else if (stage == gl::GEOMETRY_SHADER) stageBit = gl::GEOMETRY_SHADER_BIT;
    else SQASSERT(false, "invalid shader stage specified");
}

Shader::~Shader() {
    gl::DeleteProgram(program);
}

void Shader::add_uniform(const string& _name, uint _count) {
    uniforms.emplace(_name, Uniform{_count, 0});
}

void Shader::load(const string& _source) {
    const char* src = _source.c_str();
    if (program != 0u) gl::DeleteProgram(program);
    program = gl::CreateShaderProgramv(stage, 1, &src);
    int length = 0; char log[2048]; gl::GetProgramInfoLog(program, 2048, &length, log);
    if (length > 0) { string logStr(log); logStr.erase(logStr.rfind('\n'));
        log_warning("Failed to compile shader from string\n%s", logStr); }
    for (auto& uf : uniforms) uf.second.id = gl::GetUniformLocation(program, uf.first.c_str());
}

void Shader::load(const string& _source, const string& _path) {
    const char* src = _source.c_str();
    if (program != 0u) gl::DeleteProgram(program);
    program = gl::CreateShaderProgramv(stage, 1, &src);
    int length = 0; char log[2048]; gl::GetProgramInfoLog(program, 2048, &length, log);
    if (length > 0) { string logStr(log); logStr.erase(logStr.rfind('\n'));
        log_warning("Failed to compile shader from %s\n%s", _path, logStr); }
    for (auto& uf : uniforms) uf.second.id = gl::GetUniformLocation(program, uf.first.c_str());
}


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


namespace sq {

template<> void Shader::update<int>(const string& _name, const int& _val) const
{ gl::ProgramUniform1i(program, uniforms.at(_name).id, _val); }
template<> void Shader::update<uint>(const string& _name, const uint& _val) const
{ gl::ProgramUniform1ui(program, uniforms.at(_name).id, _val); }
template<> void Shader::update<float>(const string& _name, const float& _val) const
{ gl::ProgramUniform1f(program, uniforms.at(_name).id, _val); }

template<> void Shader::update<Vec2I>(const string& _name, const Vec2I& _val) const
{ gl::ProgramUniform2i(program, uniforms.at(_name).id, _val.x, _val.y); }
template<> void Shader::update<Vec2U>(const string& _name, const Vec2U& _val) const
{ gl::ProgramUniform2ui(program, uniforms.at(_name).id, _val.x, _val.y); }
template<> void Shader::update<Vec2F>(const string& _name, const Vec2F& _val) const
{ gl::ProgramUniform2f(program, uniforms.at(_name).id, _val.x, _val.y); }

template<> void Shader::update<Vec3I>(const string& _name, const Vec3I& _val) const
{ gl::ProgramUniform3i(program, uniforms.at(_name).id, _val.x, _val.y, _val.z); }
template<> void Shader::update<Vec3U>(const string& _name, const Vec3U& _val) const
{ gl::ProgramUniform3ui(program, uniforms.at(_name).id, _val.x, _val.y, _val.z); }
template<> void Shader::update<Vec3F>(const string& _name, const Vec3F& _val) const
{ gl::ProgramUniform3f(program, uniforms.at(_name).id, _val.x, _val.y, _val.z); }

template<> void Shader::update<Vec4I>(const string& _name, const Vec4I& _val) const
{ gl::ProgramUniform4i(program, uniforms.at(_name).id, _val.x, _val.y, _val.z, _val.w); }
template<> void Shader::update<Vec4U>(const string& _name, const Vec4U& _val) const
{ gl::ProgramUniform4ui(program, uniforms.at(_name).id, _val.x, _val.y, _val.z, _val.w); }
template<> void Shader::update<Vec4F>(const string& _name, const Vec4F& _val) const
{ gl::ProgramUniform4f(program, uniforms.at(_name).id, _val.x, _val.y, _val.z, _val.w); }

template<> void Shader::update<Mat2F>(const string& _name, const Mat2F& _val) const
{ gl::ProgramUniformMatrix2fv(program, uniforms.at(_name).id, 1, false, _val.ptr()); }
template<> void Shader::update<Mat23F>(const string& _name, const Mat23F& _val) const
{ gl::ProgramUniformMatrix2x3fv(program, uniforms.at(_name).id, 1, false, _val.ptr()); }
template<> void Shader::update<Mat3F>(const string& _name, const Mat3F& _val) const
{ gl::ProgramUniformMatrix3fv(program, uniforms.at(_name).id, 1, false, _val.ptr()); }
template<> void Shader::update<Mat34F>(const string& _name, const Mat34F& _val) const
{ gl::ProgramUniformMatrix3x4fv(program, uniforms.at(_name).id, 1, false, _val.ptr()); }
template<> void Shader::update<Mat4F>(const string& _name, const Mat4F& _val) const
{ gl::ProgramUniformMatrix4fv(program, uniforms.at(_name).id, 1, false, _val.ptr()); }

}
