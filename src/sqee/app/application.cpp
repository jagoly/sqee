#include <app/application.hpp>
#include <events/handler.hpp>
#include <scenes/scene.hpp>

using namespace sq;

Application::Application(glm::uvec2 _size, bool _resizable, const string& _title) {
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
    cout << "Renderer: " << renderer << endl;
    cout << "Version: " << version << endl;
    gl::Enable(gl::DEBUG_OUTPUT);
    gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS);
    gl::DebugMessageCallback(sq::debug_callback, 0);
    #endif

    window.setVerticalSyncEnabled(true);
}

int Application::run() {
    retCode = -1;
    sf::Clock clockFT;

    while (retCode == -1) {
        for (const string& _id : sceneSweep) {
            sceneIM.del(_id);
        } sceneSweep.clear();

        for (const string& _id : handlerSweep) {
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


void Application::sweep_handler(const string& _id) {
    handlerSweep.emplace(_id);
}

void Application::sweep_scene(const string& _id) {
    sceneSweep.emplace(_id);
}
