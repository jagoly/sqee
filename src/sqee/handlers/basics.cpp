#include <sqee/app/application.hpp>
#include <sqee/app/settings.hpp>
#include <sqee/handlers/basics.hpp>

using namespace sq;

bool HandlerClose::handle(const sf::Event& _event) {
    if (_event.type == sf::Event::Closed) {
        appBase->quit(0); return true;
    } return false;
}


bool HandlerResize::handle(const sf::Event& _event) {
    if (_event.type == sf::Event::Resized) {
        settings->mod<int>("app_width", _event.size.width);
        settings->mod<int>("app_height", _event.size.height);
        settings->apply(); appBase->update(); return true;
    } return false;
}
