#include <sqee/scenes/Basics.hpp>
#include <sqee/handlers/Basics.hpp>
#include <sqee/scripts/ConsoleScene.hpp>
#include <sqee/scripts/ConsoleHandler.hpp>
#include <sqee/misc/ResHolder.hpp>

#include "main/MainScene.hpp"
#include "main/MainHandler.hpp"
#include "main/Scripting.hpp"
#include "RpgApp.hpp"

using namespace sqt;

RpgApp::RpgApp() {
    append_scene<MainScene>("main");
    append_scene<sq::ConsoleScene>("console");
    append_scene<sq::BasicFPSScene>("fps");

    append_handler<sq::ConsoleHandler>("console");
    append_handler<sq::BasicCloseHandler>("close");
    append_handler<sq::BasicResizeHandler>("resize");
    append_handler<MainHandler>("main");

    cs_setup_main(*cs);
    cs_setup_wcoe(*cs);
    cs_setup_rndr(*cs);
}

void RpgApp::eval_test_init() {
    cs->eval_file(sq::res::path() + "/test_init.chai");
}
