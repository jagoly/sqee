#include <iostream>

#include <libsqee/app/application.hpp>
#include <libsqee/app/handler.hpp>
#include <libsqee/scenes/scene.hpp>

using namespace sq;

Application::Application() {
    running = true;
}

void Application::run() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    window = new sf::RenderWindow(sf::VideoMode(windowSize.x, windowSize.y),
                                  "SQEE DEMO", sf::Style::Default, settings);
    window->setVerticalSyncEnabled(true);

    while (true) {
        sf::Event event;
        while (window->pollEvent(event)) {
            for (auto& handler : handlerFList) {
                if (handler->handle(event)) {
                    break;
                }
            }
        }

        if (!running) break;

        for (auto& scene : sceneVector) {
            scene->render();
        }

        for (int s = sceneVector.size() - 1; s--;) {
            sceneVector[s]->overlay(sceneVector[s+1]->get_tex());
        }

        static sf::Sprite sprite(sceneVector.front()->get_tex());

        window->clear(sf::Color::Black);
        window->draw(sprite);
        window->display();
    }
}

void Application::attach_handler(Handler* handler) {
    handlerFList.push_front(std::unique_ptr<Handler>(std::move(handler)));
    handler->application = this;
}

void Application::append_scene(Scene* scene) {
    sceneVector.push_back(std::unique_ptr<Scene>(std::move(scene)));
}

void Application::add_entity(std::string strid, Entity* entity) {
    entityMap.insert(std::pair<std::string, std::unique_ptr<Entity>>
                     (strid, std::unique_ptr<Entity>(std::move(entity))));
    entity->application = this;
}

void Application::set_size(sf::Vector2u size) {
    windowSize = size;
    for (auto& scene : sceneVector) {
        scene->set_size(size);
    }
}
