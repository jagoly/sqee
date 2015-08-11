#include <sqee/app/Application.hpp>
#include <sqee/app/Settings.hpp>
#include <sqee/handlers/Basics.hpp>

using namespace sq;

bool BasicCloseHandler::handle(const sf::Event& _event) {
    if (_event.type == sf::Event::Closed) {
        appBase->quit(0); return true;
    } return false;
}


bool BasicResizeHandler::handle(const sf::Event& _event) {
    if (_event.type == sf::Event::Resized) {
        settings->mod<int>("winwidth", _event.size.width);
        settings->mod<int>("winheight", _event.size.height);
        appBase->update(); return true;
    } return false;
}
