#include "menus.hpp"
#include "scenegame.hpp"

namespace sqt {

SceneMenus::SceneMenus(sq::Application* _app) : sqe::SceneMenu(_app, false) {
    wHBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);

    rootWindow->Add(wHBox);
}


bool HandlerMenus::handle(sf::Event& event) {
    return false;
}

}

