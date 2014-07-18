#include <libsqee/extra/handlers.hpp>
#include "app.hpp"
#include "scenegame.hpp"

namespace sqt {

TestApp::TestApp() : sq::Application(640, 480, 0, true, true, false, "SQEE GL Test") {

    ////////////////////
    /// GL STUFF     ///
    ////////////////////

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "Version: " << version << std::endl;

    // Only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST);  // Enable depth-esting
    glDepthFunc(GL_LESS);     // depth-testing interprets a smaller value as "closer"

    ////////////////////
    /// END GL STUFF ///
    ////////////////////

    attach_handler("close", sq::HandlerPtr(new sqe::HandlerClose(this)));
    attach_handler("debug", sq::HandlerPtr(new sqe::HandlerDebug(this)));
    attach_handler("resize", sq::HandlerPtr(new sqe::HandlerResize(this)));
    attach_handler("framelimit", sq::HandlerPtr(new sqe::HandlerFramelimit(this)));

    append_scene("game", sq::ScenePtr(new SceneGame(this)));
    attach_handler("game", sq::HandlerPtr(new HandlerGame(this)));
}

}
