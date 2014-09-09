#include <iostream>
#include <iterator>

#include <gl/gl_ext_3_3.hpp>

#include <app/application.hpp>
#include <events/handler.hpp>
#include <scenes/scene.hpp>

namespace sq {

Application::Application(glm::uvec2 _size, bool _showfps,
                         bool _vsync, bool _resizable, std::string _title) {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 0;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    title = _title;

    if (_resizable)
        window = new sf::Window({_size.x, _size.y}, title,
                                 sf::Style::Default, settings);
    else
        window = new sf::Window({_size.x, _size.y}, title,
                                 sf::Style::Close | sf::Style::Titlebar, settings);

    gl::sys::LoadFunctions();
    window->setKeyRepeatEnabled(false);

    set_vsync(_vsync);
    set_showfps(_showfps);
}


void Application::set_size(glm::uvec2 _size) {
    window->setSize({_size.x, _size.y});
    for (auto& scene : sceneList) {
        scene->resize(_size);
    }
}
glm::uvec2 Application::get_size() {
    return {window->getSize().x, window->getSize().y};
}

void Application::set_vsync(bool _vsync) {
    window->setVerticalSyncEnabled(_vsync);
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

void Application::run() {
    running = true;
    sf::Clock clockFT;
    sf::Clock clockFPS;

    while (true) {
        for (auto& strId : sceneSweep) {
            sceneList.remove(sceneMap[strId]);
            sceneMap.erase(strId);
        } sceneSweep.clear();

        for (auto& strId : handlerSweep) {
            handlerFList.remove(handlerMap[strId]);
            handlerMap.erase(strId);
        } handlerSweep.clear();

        soundManager.clean();

        static sf::Event event;
        while (window->pollEvent(event)) {
            for (auto& handler : handlerFList) {
                if (handler->handle(event)) {
                    break;
                }
            }
        }
        if (!running) break;

        float ft = clockFT.restart().asSeconds();

        for (auto& scene : sceneList) {
            scene->accum += ft;
            while (scene->accum >= 1.d / double(scene->tickRate)) {
                scene->update();
                scene->accum -= 1.d / double(scene->tickRate);
            }
        }

        for (auto& scene : sceneList) {
            scene->render(ft);
        }

        if (showfps) {
            static float fps = 60.f;
            fps = fps * 0.9f + 1.f / ft * 0.1f;
            if (clockFPS.getElapsedTime().asSeconds() > 1.f) {
                window->setTitle(title + " | FPS: " + std::to_string(fps));
                clockFPS.restart();
            }
        }

        window->display();
    }
}

void Application::attach_handler(std::string strId, HandlerPtr handler) {
    handlerMap.insert(std::make_pair(strId, handler));
    handlerFList.push_front(handler);
}

void Application::append_scene(std::string strId, ScenePtr scene) {
    sceneMap.insert(std::make_pair(strId, scene));
    sceneList.push_back(scene);
}

void Application::prepend_scene(std::string strId, ScenePtr scene) {
    sceneMap.insert(std::make_pair(strId, scene));
    sceneList.push_front(scene);
}

void Application::insert_scene(int index, std::string strId, ScenePtr scene) {
    std::list<ScenePtr>::iterator iter = sceneList.begin();
    std::advance(iter, index);
    sceneMap.insert(std::make_pair(strId, scene));
    sceneList.insert(iter, scene);
}

Scene& Application::get_scene_first() {
    return *sceneList.front();
}

Scene& Application::get_scene_last() {
    return *sceneList.back();
}

Scene& Application::get_scene(std::string strId) {
    return *sceneMap[strId];
}

void Application::sweep_handler(std::string strId) {
    handlerSweep.emplace(strId);
}

void Application::sweep_scene(std::string strId) {
    sceneSweep.emplace(strId);
}

}
