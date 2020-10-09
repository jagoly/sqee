#include <sqee/gl/Program.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/gl/Functions.hpp>

using namespace sq;

//============================================================================//

Program::Program(Program&& other) noexcept
{
    *this = std::move(other);
}

Program& Program::operator=(Program&& other) noexcept
{
    std::swap(mVertexShader, other.mVertexShader);
    std::swap(mGeometryShader, other.mGeometryShader);
    std::swap(mFragmentShader, other.mFragmentShader);
    std::swap(mHandle, other.mHandle);
    return *this;
}

Program::~Program() noexcept
{
    if (mVertexShader) gl::DeleteShader(mVertexShader);
    if (mGeometryShader) gl::DeleteShader(mGeometryShader);
    if (mFragmentShader) gl::DeleteShader(mFragmentShader);
    if (mHandle) gl::DeleteProgram(mHandle);
}

//============================================================================//

void Program::load_shader(ShaderStage stage, StringView source, StringView path)
{
    GLuint* shader = nullptr;
    if (stage == ShaderStage::Vertex) shader = &mVertexShader;
    if (stage == ShaderStage::Geometry) shader = &mGeometryShader;
    if (stage == ShaderStage::Fragment) shader = &mFragmentShader;

    if (*shader) gl::DeleteShader(*shader);
    *shader = gl::CreateShader(GLenum(stage));

    const char* const srcData = source.data();
    const GLint srcLen = int(source.size());

    gl::ShaderSource(*shader, 1, &srcData, &srcLen);
    gl::CompileShader(*shader);

    GLsizei logLen = 0; GLchar logBuf[2048];
    gl::GetShaderInfoLog(*shader, 2048, &logLen, logBuf);

    if (logLen > 0)
    {
        StringView logStr = logBuf;
        while (logStr.back() == '\n') logStr.remove_suffix(1u);
        if (path.empty()) log_warning_multiline("Problem compiling shader from string\n{}", logStr);
        else log_warning_multiline("Problem compiling shader from '{}'\n{}", path, logStr);
    }
}

//============================================================================//

void Program::link_program_stages()
{
    if (mHandle) gl::DeleteProgram(mHandle);
    mHandle = gl::CreateProgram();

    SQASSERT(mVertexShader != 0u, "no vertex shader loaded");
    gl::AttachShader(mHandle, mVertexShader);

    if (mGeometryShader) gl::AttachShader(mHandle, mGeometryShader);
    if (mFragmentShader) gl::AttachShader(mHandle, mFragmentShader);

    gl::LinkProgram(mHandle);

    GLsizei logLen = 0; GLchar logBuf[2048];
    gl::GetProgramInfoLog(mHandle, 2048, &logLen, logBuf);

    if (logLen > 0)
    {
        StringView logStr = logBuf;
        while (logStr.back() == '\n') logStr.remove_suffix(1u);
        log_warning_multiline("Failed to link program\n{}", logStr);
    }
}

//============================================================================//

void Program::create(StringView vertex, StringView fragment)
{
    load_shader(ShaderStage::Vertex, vertex);
    load_shader(ShaderStage::Fragment, fragment);
    link_program_stages();
}

//============================================================================//

std::optional<Program::UniformInfo> Program::query_uniform_info(const String& name) const
{
    const GLuint index = gl::GetProgramResourceIndex(mHandle, gl::UNIFORM, name.c_str());
    if (index == gl::INVALID_INDEX) return std::nullopt;

    const GLenum keys[2] = { gl::TYPE, gl::LOCATION }; GLint values[2];
    gl::GetProgramResourceiv(mHandle, gl::UNIFORM, index, 2, keys, 2, nullptr, values);

    switch (GLenum(values[0])) {

    case gl::FLOAT: case gl::FLOAT_VEC2: case gl::FLOAT_VEC3:
        return {{ GLenum(values[0]), GLint(values[1]), 0u }};

    case gl::SAMPLER_2D: case gl::SAMPLER_2D_ARRAY: case gl::SAMPLER_CUBE:
        GLint binding;
        gl::GetUniformiv(mHandle, GLint(values[1]), &binding);
        return {{ GLenum(values[0]), GLint(values[1]), GLuint(binding) }};

    default: return std::nullopt;

    } // discard unsupported uniform types
}

//============================================================================//

void Program::update(GLint location, int sca) const
{
    SQASSERT(mHandle != 0u, "not linked");
    gl::ProgramUniform1i(mHandle, location, sca);
}

void Program::update(GLint location, uint sca) const
{
    SQASSERT(mHandle != 0u, "not linked");
    gl::ProgramUniform1ui(mHandle, location, sca);
}

void Program::update(GLint location, float sca) const
{
    SQASSERT(mHandle != 0u, "not linked");
    gl::ProgramUniform1f(mHandle, location, sca);
}

//----------------------------------------------------------------------------//

void Program::update(GLint location, Vec2I vec) const
{
    SQASSERT(mHandle != 0u, "not linked");
    gl::ProgramUniform2i(mHandle, location, vec.x, vec.y);
}

void Program::update(GLint location, Vec2U vec) const
{
    SQASSERT(mHandle != 0u, "not linked");
    gl::ProgramUniform2ui(mHandle, location, vec.x, vec.y);
}

void Program::update(GLint location, Vec2F vec) const
{
    SQASSERT(mHandle != 0u, "not linked");
    gl::ProgramUniform2f(mHandle, location, vec.x, vec.y);
}

//----------------------------------------------------------------------------//

void Program::update(GLint location, Vec3I vec) const
{
    SQASSERT(mHandle != 0u, "not linked");
    gl::ProgramUniform3i(mHandle, location, vec.x, vec.y, vec.z);
}

void Program::update(GLint location, Vec3U vec) const
{
    SQASSERT(mHandle != 0u, "not linked");
    gl::ProgramUniform3ui(mHandle, location, vec.x, vec.y, vec.z);
}

void Program::update(GLint location, Vec3F vec) const
{
    SQASSERT(mHandle != 0u, "not linked");
    gl::ProgramUniform3f(mHandle, location, vec.x, vec.y, vec.z);
}

//----------------------------------------------------------------------------//

void Program::update(GLint location, Vec4I vec) const
{
    SQASSERT(mHandle != 0u, "not linked");
    gl::ProgramUniform4i(mHandle, location, vec.x, vec.y, vec.z, vec.w);
}

void Program::update(GLint location, Vec4U vec) const
{
    SQASSERT(mHandle != 0u, "not linked");
    gl::ProgramUniform4ui(mHandle, location, vec.x, vec.y, vec.z, vec.w);
}

void Program::update(GLint location, Vec4F vec) const
{
    SQASSERT(mHandle != 0u, "not linked");
    gl::ProgramUniform4f(mHandle, location, vec.x, vec.y, vec.z, vec.w);
}

//----------------------------------------------------------------------------//

void Program::update(GLint location, const Mat3F& mat) const
{
    SQASSERT(mHandle != 0u, "not linked");
    gl::ProgramUniformMatrix3fv(mHandle, location, 1, false, mat.data);
}

void Program::update(GLint location, const Mat34F& mat) const
{
    SQASSERT(mHandle != 0u, "not linked");
    gl::ProgramUniformMatrix3x4fv(mHandle, location, 1, false, mat.data);
}

void Program::update(GLint location, const Mat4F& mat) const
{
    SQASSERT(mHandle != 0u, "not linked");
    gl::ProgramUniformMatrix4fv(mHandle, location, 1, false, mat.data);
}
