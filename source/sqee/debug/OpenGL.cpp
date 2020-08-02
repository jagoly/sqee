#include <sqee/debug/OpenGL.hpp>

#include <sqee/debug/Logging.hpp>
#include <sqee/redist/gl_loader.hpp>

#include <fmt/chrono.h>

//============================================================================//

void MSVC_STDCALL sq::debug_callback(GLenum _source, GLenum _type, GLuint id, GLenum _severity,
                                     GLsizei, const GLchar* message, const void*)
{
    const char* source   = "???";
    const char* type     = "???";
    const char* severity = "???";

    if (_source == gl::DEBUG_SOURCE_API)             source = "API";
    if (_source == gl::DEBUG_SOURCE_WINDOW_SYSTEM)   source = "WINDOW_SYSTEM";
    if (_source == gl::DEBUG_SOURCE_SHADER_COMPILER) source = "SHADER_COMPILER";
    if (_source == gl::DEBUG_SOURCE_THIRD_PARTY)     source = "THIRD_PARTY";
    if (_source == gl::DEBUG_SOURCE_APPLICATION)     source = "APPLICATION";
    if (_source == gl::DEBUG_SOURCE_OTHER)           source = "OTHER";

    if (_type == gl::DEBUG_TYPE_ERROR)               type = "ERROR";
    if (_type == gl::DEBUG_TYPE_DEPRECATED_BEHAVIOR) type = "DEPRECATED_BEHAVIOR";
    if (_type == gl::DEBUG_TYPE_UNDEFINED_BEHAVIOR)  type = "UNDEFINED_BEHAVIOR";
    if (_type == gl::DEBUG_TYPE_PORTABILITY)         type = "PORTABILITY";
    if (_type == gl::DEBUG_TYPE_PERFORMANCE)         type = "PERFORMANCE";
    if (_type == gl::DEBUG_TYPE_OTHER)               type = "OTHER";
    if (_type == gl::DEBUG_TYPE_MARKER)              type = "MARKER";
    if (_type == gl::DEBUG_TYPE_PUSH_GROUP)          type = "PUSH_GROUP";
    if (_type == gl::DEBUG_TYPE_POP_GROUP)           type = "POP_GROUP";

    if (_severity == gl::DEBUG_SEVERITY_NOTIFICATION) severity = "NOTIFICATION";
    if (_severity == gl::DEBUG_SEVERITY_HIGH)         severity = "HIGH";
    if (_severity == gl::DEBUG_SEVERITY_MEDIUM)       severity = "MEDIUM";
    if (_severity == gl::DEBUG_SEVERITY_LOW)          severity = "LOW";

    std::time_t now = std::time(nullptr);
    sq::log_raw_multiline("{:%H:%M:%S} OpenGL: Source {} | Severity {} | Type {} | ID {}\n{}",
                          fmt::localtime(now), source, severity, type, id, message);

    return;
}

//============================================================================//

void sq::debug_message(StringView message)
{
    gl::DebugMessageInsert(gl::DEBUG_SOURCE_APPLICATION, gl::DEBUG_TYPE_MARKER, 0u,
                           gl::DEBUG_SEVERITY_NOTIFICATION, int(message.size()), message.data());
}
