#include "app/logging.hpp"
#include "gl/gl_ext_3_3.hpp"
#include "gl/maths.hpp"
#include "gl/misc.hpp"
#include "scripts/intergration.hpp"

#include "app/application.hpp"

using namespace sq;

Application::Application(glm::uvec2 _size, bool _resizable) {
    sf::ContextSettings sfmlSettings;
    sfmlSettings.depthBits        = 24,
    sfmlSettings.stencilBits       = 8,
    sfmlSettings.antialiasingLevel = 0,
    sfmlSettings.majorVersion      = 3,
    sfmlSettings.minorVersion      = 3;

    window.create({_size.x, _size.y}, "",
                  _resizable ? sf::Style::Default : sf::Style::Close | sf::Style::Titlebar,
                  sfmlSettings);

    gl::sys::LoadFunctions();

    #ifdef SQEE_DEBUG
    const GLubyte* renderer = gl::GetString(gl::RENDERER);
    const GLubyte* version = gl::GetString(gl::VERSION);
    log_info("Renderer $0", (char*)renderer);
    log_info("Version $0", (char*)version);
    gl::Enable(gl::DEBUG_OUTPUT);
    gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS);
    gl::DebugMessageCallback(debug_callback, nullptr);
    #endif

    settings.add("app_vsync", false);
    settings.add("app_keyrepeat", false);
    settings.add("app_title", "SQEE Application");

    sq::cs_setup_glm(cs);
    sq::cs_setup_application(cs);
    sq::cs_setup_settings(cs);
}

int Application::run() {
    cs.add(chai::var(this), "app");

    retCode = -1;
    sf::Clock clockFT;

    while (retCode == -1) {
        for (const string& _id : sceneSweep) {
            sceneIM.del(_id);
        } sceneSweep.clear();

        for (const string& _id : handlerSweep) {
            handlerIM.del(_id);
        } handlerSweep.clear();

        sndMan.clean();

        static sf::Event event;
        while (window.pollEvent(event))
            for (Handler::Ptr& handler : handlerIM)
                if (handler->handle(event)) break;

        update_settings();

        float ft = clockFT.restart().asSeconds();

        for (Scene::Ptr& scene : sceneIM) {
            scene->accum += ft;
            double dt = 1.d / double(scene->tickRate);
            while (scene->accum >= dt) {
                scene->update();
                scene->accum -= dt;
            }
        }

        for (Scene::Ptr& scene : sceneIM) {
            scene->render(ft);
        }

        window.display();
    }

    return retCode;
}

void Application::quit(int _code) {
    retCode = _code;
}


void Application::set_size(glm::uvec2 _size) {
    window.setSize({_size.x, _size.y});
    for (Scene::Ptr& scene : sceneIM) {
        scene->resize(_size);
    }
}
glm::uvec2 Application::get_size() {
    return {window.getSize().x, window.getSize().y};
}


void Application::sweep_handler(const string& _id) {
    handlerSweep.emplace(_id);
}

void Application::sweep_scene(const string& _id) {
    sceneSweep.emplace(_id);
}

void Application::update_settings() {
    if (settings.check_update("Application")) {
        window.setVerticalSyncEnabled(settings.crnt("app_vsync").b());
        window.setKeyRepeatEnabled(settings.crnt("app_keyrepeat").b());
        window.setTitle(settings.crnt("app_title").s());
    }
}
