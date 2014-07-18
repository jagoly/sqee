#include <iostream>
#include <iterator>

#include <application.hpp>
#include <handler.hpp>
#include <scene.hpp>

namespace sq {

Application::Application(unsigned int _width, unsigned int _height, unsigned int _AA, bool _showfps,
                         bool _vsync, bool _resizable, std::string _title) {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = _AA;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    if (_resizable)
        window = new sf::RenderWindow({_width, _height}, _title,
                                      sf::Style::Default, settings);
    else
        window = new sf::RenderWindow({_width, _height}, _title,
                                      sf::Style::Close | sf::Style::Titlebar, settings);

    set_vsync(_vsync);
    set_showfps(_showfps);

}


void Application::set_size(unsigned int width, unsigned int height) {
    window->setView(sf::View({0, 0, float(width), float(height)}));
    for (auto& scene : sceneList) {
        scene->resize(width, height);
    }
}
sf::Vector2u Application::get_size() {
    return window->getSize();
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
    sf::Clock FT;

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

        sf::Event event;
        while (window->pollEvent(event)) {
            for (auto& handler : handlerFList) {
                if (handler->handle(event)) {
                    break;
                }
            }
        }
        if (!running) break;

        float ft = FT.restart().asSeconds();

        for (auto& scene : sceneList) {
            scene->accum += ft;
            while (scene->accum >= 1.d / double(scene->tickRate)) {
                scene->update();
                scene->accum -= 1.d / double(scene->tickRate);
            }
        }

        glClearColor(0.5f, 0.5f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto& scene : sceneList) {
            scene->render(*window, ft);
        }

        if (showfps) {
            static float fps = 60.f;
            fps = fps * 0.9f + 1.f / ft * 0.1f;
            static sf::Text textDisplay("", fontHolder.get_font("static"), 24);

            textDisplay.setString(" FPS: " + std::to_string(fps));

            window->pushGLStates();
            window->draw(textDisplay);
            window->popGLStates();
        }

        window->display();
    }
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

void Application::sweep_scene(std::string strId) {
    sceneSweep.emplace(strId);
}

void Application::attach_handler(std::string strId, HandlerPtr handler) {
    handlerMap.insert(std::make_pair(strId, handler));
    handlerFList.push_front(handler);
}

void Application::sweep_handler(std::string strId) {
    handlerSweep.emplace(strId);
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

}
