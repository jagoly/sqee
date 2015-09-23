#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/debug/OpenGL.hpp>

using namespace sq;

void sq::debug_callback(GLenum _source, GLenum _type, GLuint _id, GLenum _severity,
                        GLsizei _length, const GLchar* _message, const void* _param) {
    string source;
    if (_source == gl::DEBUG_SOURCE_API)
        source = "API";
    else if (_source == gl::DEBUG_SOURCE_APPLICATION)
        source = "APPLICATION";
    else if (_source == gl::DEBUG_SOURCE_OTHER)
        source = "OTHER";
    else if (_source == gl::DEBUG_SOURCE_SHADER_COMPILER)
        source = "SHADER_COMPILER";
    else if (_source == gl::DEBUG_SOURCE_THIRD_PARTY)
        source = "THIRD_PARTY";
    else if (_source == gl::DEBUG_SOURCE_WINDOW_SYSTEM)
        source = "WINDOW_SYSTEM";

    string type;
    if (_type == gl::DEBUG_TYPE_DEPRECATED_BEHAVIOR)
        type = "DEPRECATED_BEHAVIOR";
    if (_type == gl::DEBUG_TYPE_ERROR)
        type = "ERROR";
    if (_type == gl::DEBUG_TYPE_MARKER)
        type = "MARKER";
    if (_type == gl::DEBUG_TYPE_OTHER)
        type = "OTHER";
    if (_type == gl::DEBUG_TYPE_PERFORMANCE)
        type = "PERFORMANCE";
    if (_type == gl::DEBUG_TYPE_POP_GROUP)
        type = "POP_GROUP";
    if (_type == gl::DEBUG_TYPE_PORTABILITY)
        type = "PORTABILITY";
    if (_type == gl::DEBUG_TYPE_PUSH_GROUP)
        type = "PUSH_GROUP";
    if (_type == gl::DEBUG_TYPE_UNDEFINED_BEHAVIOR)
        type = "UNDEFINED_BEHAVIOR";

    string severity;
    if (_severity == gl::DEBUG_SEVERITY_HIGH)
        severity = "HIGH";
    if (_severity == gl::DEBUG_SEVERITY_LOW)
        severity = "LOW";
    if (_severity == gl::DEBUG_SEVERITY_MEDIUM)
        severity = "MEDIUM";
    if (_severity == gl::DEBUG_SEVERITY_NOTIFICATION)
        severity = "NOTIFICATION";

    sq::log_only("%s OpenGL: Source %s | Severity %s | Type %s | ID %s\n%s",
                 get_time_str(), source, severity, type, _id, _message);

    return;
}


void sq::debug_message(const string& _message) {
    gl::DebugMessageInsert(gl::DEBUG_SOURCE_APPLICATION, gl::DEBUG_TYPE_MARKER, 0,
                           gl::DEBUG_SEVERITY_NOTIFICATION, _message.size(), _message.c_str());
}
