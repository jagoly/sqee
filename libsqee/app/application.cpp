#include <iostream>

#include <libsqee/app/application.hpp>
#include <libsqee/app/eventhandler.hpp>
#include <libsqee/scenes/scene.hpp>

using namespace sq;

Application::Application() {
    running = true;
}

void Application::run() {
    window = new sf::RenderWindow(sf::VideoMode(windowSize.x, windowSize.y), "SQEE DEMO");
    window->setVerticalSyncEnabled(true);

    while (true) {
        sf::Event event;
        while (window->pollEvent(event)) {
            for (auto& handler : handlerVector) {
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
    delete window;
}

void Application::attach_handler(EventHandler* handler) {
    handlerVector.push_back(std::unique_ptr<EventHandler>(std::move(handler)));
    handler->application = this;
}

void Application::append_scene(Scene* scene) {
    sceneVector.push_back(std::unique_ptr<Scene>(std::move(scene)));
}

void Application::set_size(sf::Vector2u size) {
    windowSize = size;
    for (auto& scene : sceneVector) {
        scene->set_size(size);
    }
}
