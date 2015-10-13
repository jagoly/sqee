#include <sqee/scripts/ChaiScript.hpp>
#include <sqee/misc/ResHolder.hpp>
#include <sqee/debug/Logging.hpp>

#include "main/MainScene.hpp"
#include "main/MainHandler.hpp"
#include "main/Scripting.hpp"
#include "RpgApp.hpp"

using namespace sqt;

RpgApp::RpgApp() {
    append_scene<MainScene>("main");
    append_handler<MainHandler>("main");

    cs_setup_main(*cs);
    cs_setup_wcoe(*cs);
    cs_setup_rndr(*cs);
}

void RpgApp::eval_test_init() {
    try { cs->eval_file(sq::res::path() + "test_init.chai"); }
    catch (chai::exception::eval_error& err) {
        sq::log_error(err.pretty_print());
        quit(1);
    }
}
