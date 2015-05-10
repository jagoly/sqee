#include "sqee/app/application.hpp"
#include "sqee/events/basichandlers.hpp"

using namespace sq;

bool HandlerClose::handle(const sf::Event& _event) {
    if (_event.type == sf::Event::Closed) {
        app.quit(0);
        return true;
    } return false;
}


bool HandlerResize::handle(const sf::Event& _event) {
    if (_event.type == sf::Event::Resized) {
        app.resize_scenes({_event.size.width, _event.size.height});
        return true;
    } return false;
}
