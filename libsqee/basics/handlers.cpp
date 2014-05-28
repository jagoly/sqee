#include <iostream>

#include <libsqee/extra.hpp>

using namespace sqe;

bool HandlerClose::handle(sf::Event& event) {
    if (event.type == sf::Event::Closed) {
        application->running = false;
        return true;
    }
    return false;
}

bool HandlerDebug::handle(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::D) {
            std::cout << "NYI" << std::endl;
            return true;
        }
    }
    return false;
}
