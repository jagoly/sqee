#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/handlers/Basics.hpp>

#include "scenegame.hpp"
#include "app.hpp"

namespace sqt {

GlApp::GlApp() : sq::Application(true) {
    append_scene<SceneGame>("game");
    append_handler<sq::HandlerClose>("close");
    append_handler<sq::HandlerResize>("resize");
    append_handler<HandlerGame>("game");
}

}
