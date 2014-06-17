#include <iostream>

#include <extra/handlers.hpp>

namespace sqe {

bool HandlerClose::handle(sf::Event& event) {
    if (event.type == sf::Event::Closed) {
        app->running = false;
        return true;
    }
    return false;
}

bool HandlerDebug::handle(sf::Event& event) {
    if (SQ_KEYPRESS(D)) {
        std::cout << "NYI" << std::endl;
        return true;
    }
    return false;
}

bool HandlerResize::handle(sf::Event& event) {
    if (event.type == sf::Event::Resized) {
        app->set_size(event.size.width, event.size.height);
        return true;
    }
    return false;
}

bool HandlerFramelimit::handle(sf::Event& event) {
    if (SQ_KEYPRESS(V)) {
        app->set_vsync(!app->get_vsync());
        return true;
    }
    return false;
}

}
