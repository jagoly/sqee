#include <sqee/scripts/ChaiScript.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/app/Resources.hpp>

#include "main/MainScene.hpp"
#include "main/Scripting.hpp"
#include "RpgApp.hpp"

using namespace sqt;
namespace maths = sq::maths;

RpgApp::RpgApp() {
    append_scene<MainScene>("main");

    cs_setup_world(*cs);
    cs_setup_renderer(*cs);
    cs_setup_components(*cs);
}

void RpgApp::eval_test_init() {
    try { cs->eval_file(sq::static_path() + "test_init.chai"); }
    catch (chai::exception::eval_error& err) {
        sq::log_error(err.pretty_print());
        quit(1);
    }
}
