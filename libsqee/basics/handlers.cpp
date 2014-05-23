#include <iostream>

#include <libsqee/libsqee.hpp>
#include <libsqee/extra.hpp>

using namespace sqe;

bool HandlerClose::handle(sf::Event& event) {
    if (event.type == sf::Event::Closed) {
        std::cout << "closing" << std::endl;
        application->running = false;
        return true;
    }
    return false;
}

bool HandlerDebug::handle(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::D) {
            for (auto& entity : application->entityMap) {
                auto comp = entity.second->get_component_for_action("Info");
                std::cout << comp.actionInfo() << std::endl;
            }
            return true;
        }
    }
    return false;
}
