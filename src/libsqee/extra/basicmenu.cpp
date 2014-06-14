#include <extra/basicmenu.hpp>

using namespace sqe;

SceneMenu::SceneMenu(sq::Application* _app, bool startActive) : sq::Scene(_app) {
    rootWindow = sfg::Window::Create(sfg::Window::Style::NO_STYLE);
    desktop.Add(rootWindow);

    rootWindow->Show(startActive);
    active = startActive;
}

void SceneMenu::update() {
    rootWindow->SetAllocation({0, 0, float(app->get_size().x), float(app->get_size().y)});
}

void SceneMenu::render(sf::RenderTarget& target, float ft) {
    desktop.Update(ft);
    app->sfgui.Display(static_cast<sf::RenderWindow&>(target));
}

void SceneMenu::activate() {
    active = true;
    rootWindow->Show(true);
}

void SceneMenu::deactivate() {
    active = false;
    rootWindow->Show(false);
}

bool SceneMenu::handleEvent(sf::Event& event) {
    rootWindow->HandleEvent(event);
    return true;
}

HandlerMenu::HandlerMenu(sq::Application* _app, std::string strId) : sq::Handler(_app) {
    menuId = strId;
}

bool HandlerMenu::handle(sf::Event& event) {
    if (static_cast<SceneMenu&>(app->get_scene(menuId)).active) {
        if (event.type == sf::Event::MouseMoved ||
            event.type == sf::Event::MouseButtonPressed ||
            event.type == sf::Event::MouseButtonReleased) {
            return static_cast<SceneMenu&>(app->get_scene(menuId)).handleEvent(event);
        }
    }
    return false;
}
