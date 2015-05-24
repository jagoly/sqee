#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/handlers/basics.hpp>
#include <sqee/scripts/scene.hpp>
#include <sqee/scripts/handler.hpp>

#include "scenegame.hpp"
#include "app.hpp"

namespace sqt {

ChaiApp::ChaiApp() : sq::Application(true) {
    append_handler<sq::HandlerClose>("close");
    append_handler<sq::HandlerResize>("resize");

    append_scene<SceneGame>("game");
    append_handler<HandlerGame>("game");

    append_scene<sq::SceneConsole>("console");
    prepend_handler<sq::HandlerConsole>("console");
}

}
