#include <iostream>
#include <iterator>

#include <application.hpp>
#include <handler.hpp>
#include <scene.hpp>

using namespace sq;

Application::Application() {
    running = true;
    vsync = true;
    freeze = false;
}

void Application::run() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    window = new sf::RenderWindow(sf::VideoMode(800, 600),
                                  "SQEE DEMO", sf::Style::Default, settings);

    window->setVerticalSyncEnabled(vsync);
    //window->setFramerateLimit(30);

    sf::Clock FT;

    while (true) {
        //std::cout << "\nrender" << std::endl;
        for (auto& strId : sceneSweep) {
            sceneList.remove(sceneMap[strId]);
            sceneMap.erase(strId);
        } sceneSweep.clear();

        for (auto& strId : handlerSweep) {
            handlerFList.remove(handlerMap[strId]);
            handlerMap.erase(strId);
        } handlerSweep.clear();

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
            while (scene->accum >= 1 / double(scene->tickRate)) {
                scene->update();
                scene->accum -= 1 / double(scene->tickRate);
            }
        }

        static sf::Sprite sprite;
        window->resetGLStates();
        window->clear();
        for (auto& scene : sceneList) {
            scene->render(*window, ft);
        }

        window->display();
    }
}

void Application::set_size(sf::Vector2u size) {
    window->setView(sf::View({0, 0, static_cast<float>(size.x), static_cast<float>(size.y)}));        // CRASHES HERE
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
