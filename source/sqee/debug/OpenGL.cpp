#include <sqee/debug/OpenGL.hpp>

#include <sqee/debug/Logging.hpp>
#include <sqee/gl/Constants.hpp>
#include <sqee/gl/Functions.hpp>

#include <fmt/chrono.h>

//============================================================================//

void __stdcall sq::debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, void*)
{
    StringView strSource   = "???";
    StringView strType     = "???";
    StringView strSeverity = "???";

    if (source == gl::DEBUG_SOURCE_API)             strSource = "API";
    if (source == gl::DEBUG_SOURCE_WINDOW_SYSTEM)   strSource = "WINDOW_SYSTEM";
    if (source == gl::DEBUG_SOURCE_SHADER_COMPILER) strSource = "SHADER_COMPILER";
    if (source == gl::DEBUG_SOURCE_THIRD_PARTY)     strSource = "THIRD_PARTY";
    if (source == gl::DEBUG_SOURCE_APPLICATION)     strSource = "APPLICATION";
    if (source == gl::DEBUG_SOURCE_OTHER)           strSource = "OTHER";

    if (type == gl::DEBUG_TYPE_ERROR)               strType = "ERROR";
    if (type == gl::DEBUG_TYPE_DEPRECATED_BEHAVIOR) strType = "DEPRECATED_BEHAVIOR";
    if (type == gl::DEBUG_TYPE_UNDEFINED_BEHAVIOR)  strType = "UNDEFINED_BEHAVIOR";
    if (type == gl::DEBUG_TYPE_PORTABILITY)         strType = "PORTABILITY";
    if (type == gl::DEBUG_TYPE_PERFORMANCE)         strType = "PERFORMANCE";
    if (type == gl::DEBUG_TYPE_OTHER)               strType = "OTHER";
    if (type == gl::DEBUG_TYPE_MARKER)              strType = "MARKER";
    if (type == gl::DEBUG_TYPE_PUSH_GROUP)          strType = "PUSH_GROUP";
    if (type == gl::DEBUG_TYPE_POP_GROUP)           strType = "POP_GROUP";

    if (severity == gl::DEBUG_SEVERITY_NOTIFICATION) strSeverity = "NOTIFICATION";
    if (severity == gl::DEBUG_SEVERITY_HIGH)         strSeverity = "HIGH";
    if (severity == gl::DEBUG_SEVERITY_MEDIUM)       strSeverity = "MEDIUM";
    if (severity == gl::DEBUG_SEVERITY_LOW)          strSeverity = "LOW";

    std::time_t now = std::time(nullptr);
    sq::log_raw_multiline("{:%H:%M:%S} OpenGL: Source {} | Type {} | Severity {} | ID {}\n{}",
                          fmt::localtime(now), strSource, strType, strSeverity, id, StringView(message, size_t(length)));

    return;
}

//============================================================================//

void sq::debug_message(StringView message)
{
    gl::DebugMessageInsert(gl::DEBUG_SOURCE_APPLICATION, gl::DEBUG_TYPE_MARKER, 0u,
                           gl::DEBUG_SEVERITY_NOTIFICATION, int(message.size()), message.data());
}
