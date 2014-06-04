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

    clock.restart();
    while (true) {
        //std::cout << "\nrender" << std::endl;

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
            while (scene->accum >= 1.f / scene->tickRate) {
                scene->update();
                scene->accum -= 1.f / scene->tickRate;
            }
        }

        static sf::Sprite sprite;
        for (auto& scene : sceneList) {           // TRY OVERLAY METHOD
            scene->render(*window, ft);
        }

        window->display();
    }
}

void Application::set_size(sf::Vector2u size) {
    window->setView(sf::View({0, 0, static_cast<float>(size.x), static_cast<float>(size.y)}));        // CRASHES HERE
}

Scene& Application::get_scene() {
    return *sceneList.back();
}

//Scene& Application::get_scene(int index) {
//    return *sceneList.begin();
//}

Scene& Application::get_scene(std::string strId) {
    return *sceneMap[strId];
}

void Application::attach_handler(std::unique_ptr<Handler> handler) {
    handlerFList.push_front(std::move(handler));
}

void Application::append_scene(std::string strId, std::shared_ptr<Scene> scene) {
    sceneList.push_back(scene);
    sceneMap.insert(std::make_pair(strId, scene));
}

void Application::prepend_scene(std::string strId, std::shared_ptr<Scene> scene) {
    sceneList.push_front(scene);
    sceneMap.insert(std::make_pair(strId, scene));
}

void Application::insert_scene(int index, std::string strId, std::shared_ptr<Scene> scene) {
    std::list<std::shared_ptr<Scene>>::iterator iter = sceneList.begin();
    std::advance(iter, index);
    sceneList.insert(iter, scene);
    sceneMap.insert(std::make_pair(strId, scene));
}
