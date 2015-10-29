#include <sqee/scripts/ChaiScript.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/app/Resources.hpp>

#include "main/MainScene.hpp"
#include "main/MainHandler.hpp"
#include "main/Scripting.hpp"
#include "RpgApp.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace sqt;
namespace maths = sq::maths;

RpgApp::RpgApp() {
//    Mat4F lookMat = maths::look_at(Vec3F(0.f, 0.f, 0.3f), {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f});
//    std::cout << sq::chai_string(lookMat) << std::endl;

//    glm::fmat4 lookMatG = glm::lookAt(glm::fvec3(0.f, 0.f, 0.3f), {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f});
//    std::cout << glm::to_string(lookMatG) << std::endl;

//    Mat4F prspMat = maths::perspective(1.f, 16.f/9.f, 0.5f, 50.f);
//    std::cout << sq::chai_string(prspMat) << std::endl;

//    glm::fmat4 prspMatG = glm::perspective(1.f, 16.f/9.f, 0.5f, 50.f);
//    std::cout << glm::to_string(prspMatG) << std::endl;

//    Mat4F multMat = prspMat * lookMat;
//    std::cout << sq::chai_string(multMat) << std::endl;

//    glm::fmat4 multMatG = prspMatG * lookMatG;
//    std::cout << glm::to_string(multMatG) << std::endl;

//    QuatF quat(maths::radians(Vec3F(90.f, 90.f, 0.f)));
//    std::cout << sq::chai_string(Mat4F(quat*quat)) << std::endl;

//    glm::fquat quatG(glm::radians(glm::fvec3(90.f, 90.f, 0.f)));
//    std::cout << glm::to_string(glm::fmat4(quatG*quatG)) << std::endl;

//    std::cout << sq::chai_string(maths::inverse(lookMat)) << std::endl;
//    std::cout << glm::to_string(glm::inverse(lookMatG)) << std::endl;

//    exit(0);

    std::min<float>(5, 4);

    append_scene<MainScene>("main");
    append_handler<MainHandler>("main");

    cs_setup_main(*cs);
    cs_setup_wcoe(*cs);
    cs_setup_rndr(*cs);
}

void RpgApp::eval_test_init() {
    try { cs->eval_file(sq::static_path() + "test_init.chai"); }
    catch (chai::exception::eval_error& err) {
        sq::log_error(err.pretty_print());
        quit(1);
    }
}
