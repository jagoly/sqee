#include "handlermainmenu.hpp"
#include "scenemainmenu.hpp"

using namespace sqt;

bool HandlerMainMenu::handle(sf::Event& event) {
    if (event.type == sf::Event::MouseMoved ||
        event.type == sf::Event::MouseButtonPressed ||
        event.type == sf::Event::MouseButtonReleased) {
        static_cast<SceneMainMenu&>(app->get_scene("mainmenu")).window->HandleEvent(event);
    }
    return false;
}
