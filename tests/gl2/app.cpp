#include <iostream>

#include <libsqee/events/basichandlers.hpp>
#include "app.hpp"
#include "scenegame.hpp"

namespace sqt {

void debugCallback(GLenum _source, GLenum _type, GLuint /*_id*/, GLenum _severity,
                   GLsizei /*_length*/, const GLchar* _message, const void* /*_param*/) {
    std::string source;
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

    std::string type;
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

    std::string severity;
    if (_severity == gl::DEBUG_SEVERITY_HIGH)
        severity = "HIGH";
    if (_severity == gl::DEBUG_SEVERITY_LOW)
        severity = "LOW";
    if (_severity == gl::DEBUG_SEVERITY_MEDIUM)
        severity = "MEDIUM";
    if (_severity == gl::DEBUG_SEVERITY_NOTIFICATION)
        severity = "NOTIFICATION";

    std::cout << "\nSource: " << source;
    std::cout << "\nSeverity: " << severity;
    std::cout << "\nType: " << type;
    std::cout << "\nMessage: " << _message;
    std::cout << std::endl;
}

TestApp::TestApp() : sq::Application({1280, 720}, true, true, true, "SQEE GL Test Two") {
    const GLubyte* renderer = gl::GetString(gl::RENDERER);
    const GLubyte* version = gl::GetString(gl::VERSION);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "Version: " << version << std::endl;

    gl::Enable(gl::DEBUG_OUTPUT);
    gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS);
    gl::DebugMessageCallback(debugCallback, 0);

    attach_handler("close", sq::HandlerPtr(new sq::HandlerClose(this)));
    attach_handler("debug", sq::HandlerPtr(new sq::HandlerDebug(this)));
    attach_handler("resize", sq::HandlerPtr(new sq::HandlerResize(this)));
    attach_handler("framelimit", sq::HandlerPtr(new sq::HandlerFramelimit(this)));

    append_scene("game", sq::ScenePtr(new SceneGame(this)));
    attach_handler("game", sq::HandlerPtr(new HandlerGame(this)));
}

}
