#include <iostream>
#include <iterator>

#include <gl/gl_ext_3_3.hpp>

#include <app/application.hpp>
#include <events/handler.hpp>
#include <scenes/scene.hpp>

namespace sq {

Application::Application(glm::uvec2 _size, bool _showFPS,
                         bool _vSync, bool _resizable, const std::string& _title) {
    int error = FT_Init_FreeType(&ftLib);
    if (error) std::cout << "ERROR: Failed to initialise FreeType" << std::endl;

    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 0;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    title = _title;

    window.create({_size.x, _size.y}, title,
                  _resizable ? sf::Style::Default : sf::Style::Close | sf::Style::Titlebar,
                  settings);
    window.setKeyRepeatEnabled(false);

    gl::sys::LoadFunctions();

    const GLubyte* renderer = gl::GetString(gl::RENDERER);
    const GLubyte* version = gl::GetString(gl::VERSION);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "Version: " << version << std::endl;
#ifdef SQEE_DEBUG
    gl::Enable(gl::DEBUG_OUTPUT);
    gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS);
    gl::DebugMessageCallback(sq::debug_callback, 0);
#endif

    set_vSync(_vSync);
    set_showFPS(_showFPS);
}

void Application::run() {
    running = true;
    sf::Clock clockFT;

    while (running) {
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
            while (scene->accum >= 1.d / double(scene->tickRate)) {
                scene->update();
                scene->accum -= 1.d / double(scene->tickRate);
            }
        }

        for (Scene::Ptr& scene : sceneIM) {
            scene->render(ft);
        }

        if (showFPS) {
            static float fps = 60.f;
            fps = fps * 0.9f + 1.f / ft * 0.1f;
            // set something up
        }

        window.display();
    }
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

void Application::set_vSync(bool _vSync) {
    window.setVerticalSyncEnabled(_vSync);
    vSync = _vSync;
}
bool Application::get_vSync() {
    return vSync;
}

void Application::set_showFPS(bool _showFPS) {
    showFPS = _showFPS;
}
bool Application::get_showFPS() {
    return showFPS;
}


void Application::sweep_handler(const std::string& _id) {
    handlerSweep.emplace(_id);
}

void Application::sweep_scene(const std::string& _id) {
    sceneSweep.emplace(_id);
}

}
