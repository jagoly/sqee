#ifdef SQEE_DEBUG
#include "debug/glcallback.hpp"
#endif

#include "app/logging.hpp"
#include "gl/gl_ext_3_3.hpp"
#include "scripts/intergration.hpp"
#include "app/application.hpp"

using namespace sq;

Application::Application(uvec2 _size, bool _resizable) {
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

    settings.add<bool>("app_vsync", false);
    settings.add<bool>("app_keyrepeat", false);
    settings.add<string>("app_title", "SQEE Application");

    sq::cs_setup_glm(cs);
    sq::cs_setup_application(cs);
    sq::cs_setup_settings(cs);
}

int Application::run() {
    cs.add(chai::var(this), "app");

    retCode = -1;
    sf::Clock clockFT;

    set_size(get_size());

    while (retCode == -1) {
        for (auto& _id : sceneSweep)
            sceneIM.del(_id);
        sceneSweep.clear();

        for (auto& _id : handlerSweep)
            handlerIM.del(_id);
        handlerSweep.clear();

        sndMan.clean();

        static sf::Event event;
        while (window.pollEvent(event))
            for (auto& handler : handlerIM)
                if (handler->handle(event)) break;

        update_settings();

        float ft = clockFT.restart().asSeconds();

        for (auto& scene : sceneIM) {
            scene->accum += ft;
            double dt = 1.0 / double(scene->tickRate);
            while (scene->accum >= dt) {
                scene->update();
                scene->accum -= dt;
            }
        }

        for (auto& scene : sceneIM) {
            scene->render(ft);
        }

        window.display();
    }

    return retCode;
}

void Application::quit(int _code) {
    retCode = _code;
}

void Application::set_size(uvec2 _size) {
    window.setSize({_size.x, _size.y});
    for (auto& scene : sceneIM) {
        scene->resize(_size);
    }
}

uvec2 Application::get_size() {
    return {window.getSize().x, window.getSize().y};
}

vec2 Application::mouse_relatify() {
    sf::Vector2i winCentre(window.getSize() / 2u);
    sf::Vector2i mouseMove = winCentre - sf::Mouse::getPosition(window);
    sf::Mouse::setPosition(winCentre, window);
    return glm::vec2(mouseMove.x, mouseMove.y);
}

void Application::sweep_handler(const string& _id) {
    handlerSweep.emplace(_id);
}

void Application::sweep_scene(const string& _id) {
    sceneSweep.emplace(_id);
}

void Application::update_settings() {
    if (settings.check_update("Application")) {
        window.setVerticalSyncEnabled(settings.crnt<bool>("app_vsync"));
        window.setKeyRepeatEnabled(settings.crnt<bool>("app_keyrepeat"));
        window.setTitle(settings.crnt<string>("app_title"));
    }
}
