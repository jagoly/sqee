#include <sqee/gl/Program.hpp>

#include <sqee/debug/Logging.hpp>
#include <sqee/gl/Context.hpp>
#include <sqee/redist/gl_loader.hpp>

using namespace sq;

//============================================================================//

namespace { // anonymous

void impl_load_shader(GLuint& shader, GLenum stage, StringView source, StringView path)
{
    if (shader != 0u) gl::DeleteShader(shader);
    shader = gl::CreateShader(stage);

    const GLchar* srcData = source.data();
    const GLint srcLen = int(source.size());

    gl::ShaderSource(shader, 1, &srcData, &srcLen);
    gl::CompileShader(shader);

    //--------------------------------------------------------//

    GLsizei logLen = 0; GLchar logBuf[2048];
    gl::GetShaderInfoLog(shader, 2048, &logLen, logBuf);

    if (logLen > 0)
    {
        StringView logStr = logBuf;
        while (logStr.back() == '\n') logStr.remove_suffix(1u);
        log_warning_multiline("Problem compiling shader from '{}'\n{}", path, logStr);
    }
}

} // anonymous namespace

//============================================================================//

Program::Program() : mContext(Context::get()) {}

//============================================================================//

Program::Program(Program&& other) noexcept
    : mContext(other.mContext)
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

Program& Program::operator=(Program&& other) noexcept
{ std::swap(*this, other); return *this; }

//============================================================================//

Program::~Program() noexcept
{
    mContext.impl_reset_Program(this);

    if (mVertexShader) gl::DeleteShader(mVertexShader);
    if (mGeometryShader) gl::DeleteShader(mGeometryShader);
    if (mFragmentShader) gl::DeleteShader(mFragmentShader);
    if (mHandle) gl::DeleteProgram(mHandle);
}

//============================================================================//

void Program::load_vertex(StringView source, StringView path)
{
    impl_load_shader(mVertexShader, gl::VERTEX_SHADER, source, path);
}

void Program::load_geometry(StringView source, StringView path)
{
    impl_load_shader(mGeometryShader, gl::GEOMETRY_SHADER, source, path);
}

void Program::load_fragment(StringView source, StringView path)
{
    impl_load_shader(mFragmentShader, gl::FRAGMENT_SHADER, source, path);
}

//============================================================================//

void Program::link_program_stages()
{
    if (mHandle != 0u) gl::DeleteProgram(mHandle);
    mHandle = gl::CreateProgram();

    if (mVertexShader != 0u) gl::AttachShader(mHandle, mVertexShader);
    if (mGeometryShader != 0u) gl::AttachShader(mHandle, mGeometryShader);
    if (mFragmentShader != 0u) gl::AttachShader(mHandle, mFragmentShader);

    gl::LinkProgram(mHandle);

    //--------------------------------------------------------//

    GLsizei logLen = 0; GLchar logBuf[2048];
    gl::GetProgramInfoLog(mHandle, 2048, &logLen, logBuf);

    if (logLen > 0)
    {
        StringView logStr = logBuf;
        while (logStr.back() == '\n') logStr.remove_suffix(1u);
        log_warning_multiline("Failed to link shader\n{}", logStr);
    }
}

//============================================================================//

void Program::create(StringView vertex, StringView fragment)
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
{ gl::ProgramUniformMatrix3fv(mHandle, location, 1, false, mat.data); }

void Program::update(GLint location, const Mat34F& mat) const
{ gl::ProgramUniformMatrix3x4fv(mHandle, location, 1, false, mat.data); }

void Program::update(GLint location, const Mat4F& mat) const
{ gl::ProgramUniformMatrix4fv(mHandle, location, 1, false, mat.data); }
