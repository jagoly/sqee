#include <iostream>

#include <gl/gl_ext_3_3.hpp>
#include <app/application.hpp>
#include <events/handler.hpp>
#include <scenes/scene.hpp>

using namespace sq;

Application::Application(glm::uvec2 _size, bool _showfps, bool _vsync,
                         bool _resizable, const std::string& _title) {
    int error = FT_Init_FreeType(&ftLib);
    if (error) std::cout << "ERROR: Failed to initialise FreeType" << std::endl;

    sf::ContextSettings settings;
    settings.depthBits        = 24;
    settings.stencilBits       = 8;
    settings.antialiasingLevel = 0;
    settings.majorVersion      = 3;
    settings.minorVersion      = 3;

    title = _title;

    window.create({_size.x, _size.y}, title,
                  _resizable ? sf::Style::Default : sf::Style::Close | sf::Style::Titlebar,
                  settings);
    window.setKeyRepeatEnabled(false);

    gl::sys::LoadFunctions();

#ifdef SQEE_DEBUG
    const GLubyte* renderer = gl::GetString(gl::RENDERER);
    const GLubyte* version = gl::GetString(gl::VERSION);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "Version: " << version << std::endl;
    gl::Enable(gl::DEBUG_OUTPUT);
    gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS);
    gl::DebugMessageCallback(sq::debug_callback, 0);
#endif

    set_vsync(_vsync);
    set_showfps(_showfps);
}

int Application::run() {
    retCode = -1;
    sf::Clock clockFT;

    while (retCode == -1) {
        for (const std::string& _id : sceneSweep) {
            sceneIM.del(_id);
        } sceneSweep.clear();

        for (const std::string& _id : handlerSweep) {
            handlerIM.del(_id);
        } handlerSweep.clear();

        soundManager.clean();

        static sf::Event event;
        while (window.pollEvent(event)) {
            for (Handler::Ptr& handler : handlerIM) {
                if (handler->handle(event)) {
                    break;
                }
            }
        }

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

void Application::set_vsync(bool _vsync) {
    window.setVerticalSyncEnabled(_vsync);
    vsync = _vsync;
}
bool Application::get_vsync() {
    return vsync;
}

void Application::set_showfps(bool _showfps) {
    showfps = _showfps;
}
bool Application::get_showfps() {
    return showfps;
}


void Application::sweep_handler(const std::string& _id) {
    handlerSweep.emplace(_id);
}

void Application::sweep_scene(const std::string& _id) {
    sceneSweep.emplace(_id);
}
