#include <iostream>

#include <basics/handlers.hpp>

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

bool HandlerResize::handle(sf::Event& event) {
    if (event.type == sf::Event::Resized) {
        application->set_size({event.size.width, event.size.height});
        return true;
    }
    return false;
}

bool HandlerFramelimit::handle(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::V) {
            application->window->setVerticalSyncEnabled(!application->vsync);
            application->vsync = !application->vsync;
            return true;
        }
    }
    return false;
}

//bool HandlerFreeze::handle(sf::Event& event) {    // Needs Git SFML
//    if (event.type == sf::Event::LostFocus) {
//        application->freeze = true;
//        return true;
//    }
//    return false;
//}
