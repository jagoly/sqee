#include <sqee/debug/Logging.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>

#include <sqee/gl/Context.hpp>
#include <sqee/gl/Shaders.hpp>

using namespace sq;

//============================================================================//

Shader::Shader(Stage stage) : mContext(Context::get())
{
    if (stage == Stage::Vertex) mShaderType = gl::VERTEX_SHADER;
    if (stage == Stage::Geometry) mShaderType = gl::GEOMETRY_SHADER;
    if (stage == Stage::Fragment) mShaderType = gl::FRAGMENT_SHADER;
}

Shader::Shader(Shader&& other) : mContext(other.mContext)
{
    mContext.impl_reset_Shader(&other, this);

    mShaderType = other.mShaderType;
    mUniforms = std::move(other.mUniforms);

    mHandle = other.mHandle;
    other.mHandle = 0u;
}

Shader::~Shader()
{
    if (mHandle != 0u)
    {
        mContext.impl_reset_Shader(this);
        gl::DeleteBuffers(1, &mHandle);
    }
}

//============================================================================//

void Shader::add_uniform(const string& name, uint count)
{
    mUniforms.emplace(name, Uniform{count, 0});
}

//============================================================================//

void Shader::load(const string& source)
{
    if (mHandle != 0u) gl::DeleteProgram(mHandle);

    const char* src = source.c_str();
    mHandle = gl::CreateShaderProgramv(mShaderType, 1, &src);

    int length = 0; char log[2048];
    gl::GetProgramInfoLog(mHandle, 2048, &length, log);

    if (length > 0)
    {
        string logStr(log); logStr.erase(logStr.rfind('\n'));
        log_warning("Failed to compile shader from string\n%s", logStr);
        return;
    }

    for (auto& uf : mUniforms)
    {
        uf.second.id = gl::GetUniformLocation(mHandle, uf.first.c_str());
    }
}

void Shader::load(const string& source, const string& path)
{
    if (mHandle != 0u) gl::DeleteProgram(mHandle);

    const char* src = source.c_str();
    mHandle = gl::CreateShaderProgramv(mShaderType, 1, &src);

    int length = 0; char log[2048];
    gl::GetProgramInfoLog(mHandle, 2048, &length, log);

    if (length > 0)
    {
        string logStr(log); logStr.erase(logStr.rfind('\n'));
        log_warning("Failed to compile shader from %s\n%s", path, logStr);
        return;
    }

    for (auto& uf : mUniforms)
    {
        uf.second.id = gl::GetUniformLocation(mHandle, uf.first.c_str());
    }
}

//============================================================================//

namespace sq {

template<> void Shader::update<int>(const string& name, const int& val) const
{ gl::ProgramUniform1i(mHandle, mUniforms.at(name).id, val); }
template<> void Shader::update<uint>(const string& name, const uint& val) const
{ gl::ProgramUniform1ui(mHandle, mUniforms.at(name).id, val); }
template<> void Shader::update<float>(const string& name, const float& val) const
{ gl::ProgramUniform1f(mHandle, mUniforms.at(name).id, val); }

template<> void Shader::update<Vec2I>(const string& name, const Vec2I& val) const
{ gl::ProgramUniform2i(mHandle, mUniforms.at(name).id, val.x, val.y); }
template<> void Shader::update<Vec2U>(const string& name, const Vec2U& val) const
{ gl::ProgramUniform2ui(mHandle, mUniforms.at(name).id, val.x, val.y); }
template<> void Shader::update<Vec2F>(const string& name, const Vec2F& val) const
{ gl::ProgramUniform2f(mHandle, mUniforms.at(name).id, val.x, val.y); }

template<> void Shader::update<Vec3I>(const string& name, const Vec3I& val) const
{ gl::ProgramUniform3i(mHandle, mUniforms.at(name).id, val.x, val.y, val.z); }
template<> void Shader::update<Vec3U>(const string& name, const Vec3U& val) const
{ gl::ProgramUniform3ui(mHandle, mUniforms.at(name).id, val.x, val.y, val.z); }
template<> void Shader::update<Vec3F>(const string& name, const Vec3F& val) const
{ gl::ProgramUniform3f(mHandle, mUniforms.at(name).id, val.x, val.y, val.z); }

template<> void Shader::update<Vec4I>(const string& name, const Vec4I& val) const
{ gl::ProgramUniform4i(mHandle, mUniforms.at(name).id, val.x, val.y, val.z, val.w); }
template<> void Shader::update<Vec4U>(const string& name, const Vec4U& val) const
{ gl::ProgramUniform4ui(mHandle, mUniforms.at(name).id, val.x, val.y, val.z, val.w); }
template<> void Shader::update<Vec4F>(const string& name, const Vec4F& val) const
{ gl::ProgramUniform4f(mHandle, mUniforms.at(name).id, val.x, val.y, val.z, val.w); }

template<> void Shader::update<Mat2F>(const string& name, const Mat2F& val) const
{ gl::ProgramUniformMatrix2fv(mHandle, mUniforms.at(name).id, 1, false, val.data()); }
template<> void Shader::update<Mat23F>(const string& name, const Mat23F& val) const
{ gl::ProgramUniformMatrix2x3fv(mHandle, mUniforms.at(name).id, 1, false, val.data()); }
template<> void Shader::update<Mat3F>(const string& name, const Mat3F& val) const
{ gl::ProgramUniformMatrix3fv(mHandle, mUniforms.at(name).id, 1, false, val.data()); }
template<> void Shader::update<Mat34F>(const string& name, const Mat34F& val) const
{ gl::ProgramUniformMatrix3x4fv(mHandle, mUniforms.at(name).id, 1, false, val.data()); }
template<> void Shader::update<Mat4F>(const string& name, const Mat4F& val) const
{ gl::ProgramUniformMatrix4fv(mHandle, mUniforms.at(name).id, 1, false, val.data()); }

} // namespace sq
