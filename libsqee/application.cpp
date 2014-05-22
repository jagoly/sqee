#include <iostream>

#include "application.hpp"
#include "eventhandler.hpp"
#include "scene.hpp"

using namespace sq;

Application::Application() {
    running = true;
    scene = new Scene;
}

void Application::run() {
    window = new sf::RenderWindow(sf::VideoMode(800, 600), "SQEE DEMO");
    window->setVerticalSyncEnabled(true);

    scene->set_size({800, 600});

    while (running) {
        sf::Event event;
        while (window->pollEvent(event)) {
            for (auto& handler : handlerVector) {
                if (handler->handle(event)) {
                    break;
                }
            }
        }

        scene->render();
        static sf::Sprite sprite(scene->get_tex());
        window->clear(sf::Color::White);
        window->draw(sprite);
        window->display();
    }
}

void Application::attach_handler(EventHandler* handler) {
    handlerVector.push_back(handler);
    handler->application = this;

    //std::cout << typeid(handler).name() << std::endl;

    //handler.application = this;
   // handlerVector.push_back(std::move(handler));
}
