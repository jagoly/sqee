#include <sqee/debug/Logging.hpp>

#include <sqee/gl/Context.hpp>
#include <sqee/gl/Program.hpp>

using namespace sq;

//============================================================================//

namespace { // anonymous

void impl_load_shader(GLuint& shader, GLenum stage, const string& source, const string& path)
{
    if (shader != 0u) gl::DeleteShader(shader);
    shader = gl::CreateShader(stage);

    const char* src = source.c_str();
    gl::ShaderSource(shader, 1, &src, nullptr);

    gl::CompileShader(shader);

    //--------------------------------------------------------//

    GLsizei length = 0; GLchar log[2048];
    gl::GetShaderInfoLog(shader, 2048, &length, log);

    if (length > 0)
    {
        string logStr(log); logStr.erase(logStr.rfind('\n'));
        log_error("Failed to compile shader from \"%s\"\n%s", path, logStr);
    }
}

} // anonymous namespace

//============================================================================//

Program::Program() : mContext(Context::get()) {}

//============================================================================//

Program::Program(Program&& other) : mContext(other.mContext)
{
    mContext.impl_reset_Program(&other, this);

    mVertexShader = other.mVertexShader;
    mGeometryShader = other.mGeometryShader;
    mFragmentShader = other.mFragmentShader;
    mHandle = other.mHandle;

    other.mVertexShader = 0u;
    other.mGeometryShader = 0u;
    other.mFragmentShader = 0u;
    other.mHandle = 0u;
}

Program& Program::operator=(Program&& other)
{ std::swap(*this, other); return *this; }

//============================================================================//

Program::~Program()
{
    mContext.impl_reset_Program(this);

    if (mVertexShader) gl::DeleteShader(mVertexShader);
    if (mGeometryShader) gl::DeleteShader(mGeometryShader);
    if (mFragmentShader) gl::DeleteShader(mFragmentShader);
    if (mHandle) gl::DeleteProgram(mHandle);
}

//============================================================================//

void Program::load_vertex(const string& source, const string& path)
{
    impl_load_shader(mVertexShader, gl::VERTEX_SHADER, source, path);
}

void Program::load_geometry(const string& source, const string& path)
{
    impl_load_shader(mGeometryShader, gl::GEOMETRY_SHADER, source, path);
}

void Program::load_fragment(const string& source, const string& path)
{
    impl_load_shader(mFragmentShader, gl::FRAGMENT_SHADER, source, path);
}

//============================================================================//

void Program::link_program_stages()
{
    if (mHandle != 0u) gl::DeleteProgram(mHandle);
    mHandle = gl::CreateProgram();

    if (mVertexShader   != 0u) gl::AttachShader(mHandle, mVertexShader);
    if (mGeometryShader != 0u) gl::AttachShader(mHandle, mGeometryShader);
    if (mFragmentShader != 0u) gl::AttachShader(mHandle, mFragmentShader);

    gl::LinkProgram(mHandle);

    //--------------------------------------------------------//

    GLsizei length = 0; GLchar log[2048];
    gl::GetProgramInfoLog(mHandle, 2048, &length, log);

    if (length > 0)
    {
        string logStr(log); logStr.erase(logStr.rfind('\n'));
        log_error("Failed to link shader\n%s", logStr);
    }
}

//============================================================================//

void Program::create(const string& vertex, const string& fragment)
{
    load_vertex(vertex);
    load_fragment(fragment);
    link_program_stages();
}

//============================================================================//

void Program::update(GLint location, int sca) const
{ gl::ProgramUniform1i(mHandle, location, sca); }

void Program::update(GLint location, uint sca) const
{ gl::ProgramUniform1ui(mHandle, location, sca); }

void Program::update(GLint location, float sca) const
{ gl::ProgramUniform1f(mHandle, location, sca); }

//----------------------------------------------------------------------------//

void Program::update(GLint location, Vec2I vec) const
{ gl::ProgramUniform2i(mHandle, location, vec.x, vec.y); }

void Program::update(GLint location, Vec2U vec) const
{ gl::ProgramUniform2ui(mHandle, location, vec.x, vec.y); }

void Program::update(GLint location, Vec2F vec) const
{ gl::ProgramUniform2f(mHandle, location, vec.x, vec.y); }

//----------------------------------------------------------------------------//

void Program::update(GLint location, Vec3I vec) const
{ gl::ProgramUniform3i(mHandle, location, vec.x, vec.y, vec.z); }

void Program::update(GLint location, Vec3U vec) const
{ gl::ProgramUniform3ui(mHandle, location, vec.x, vec.y, vec.z); }

void Program::update(GLint location, Vec3F vec) const
{ gl::ProgramUniform3f(mHandle, location, vec.x, vec.y, vec.z); }

//----------------------------------------------------------------------------//

void Program::update(GLint location, Vec4I vec) const
{ gl::ProgramUniform4i(mHandle, location, vec.x, vec.y, vec.z, vec.w); }

void Program::update(GLint location, Vec4U vec) const
{ gl::ProgramUniform4ui(mHandle, location, vec.x, vec.y, vec.z, vec.w); }

void Program::update(GLint location, Vec4F vec) const
{ gl::ProgramUniform4f(mHandle, location, vec.x, vec.y, vec.z, vec.w); }

//----------------------------------------------------------------------------//

void Program::update(GLint location, const Mat3F& mat) const
{ gl::ProgramUniformMatrix3fv(mHandle, location, 1, false, mat.data()); }

void Program::update(GLint location, const Mat34F& mat) const
{ gl::ProgramUniformMatrix3x4fv(mHandle, location, 1, false, mat.data()); }

void Program::update(GLint location, const Mat4F& mat) const
{ gl::ProgramUniformMatrix4fv(mHandle, location, 1, false, mat.data()); }
