#include <events/basichandlers.hpp>

using namespace sq;

bool HandlerClose::handle(const sf::Event& _event) {
    if (_event.type == sf::Event::Closed) {
        app.quit(0);
        return true;
    }
    return false;
}

bool HandlerResize::handle(const sf::Event& _event) {
    if (_event.type == sf::Event::Resized) {
        app.set_size({_event.size.width, _event.size.height});
        return true;
    }
    return false;
}

bool HandlerFramelimit::handle(const sf::Event& _event) {
    if (_event.type == sf::Event::KeyPressed && _event.key.code == sf::Keyboard::V) {
        app.set_vsync(!app.get_vsync());
        return true;
    }
    return false;
}
