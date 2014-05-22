#include <iostream>

#include <libsqee/basics/handlers.hpp>

using namespace sqe;

bool HandlerClose::handle(sf::Event& event) {
    if (event.type == sf::Event::Closed) {
        std::cout << "closing" << std::endl;
        application->running = false;
        return true;
    }
    return false;
}
