#include <extra/basicmenu.hpp>

using namespace sqe;

SceneMenu::SceneMenu(sq::Application* _app) : sq::Scene(_app) {
    rootWindow = sfg::Window::Create(sfg::Window::Style::NO_STYLE);
}

void SceneMenu::update() {
    rootWindow->SetAllocation({0, 0,
                           static_cast<float>(app->window->getSize().x),
                           static_cast<float>(app->window->getSize().y)});
}

void SceneMenu::render(sf::RenderTarget& target, float ft) {
    desktop.Update(ft);
    app->sfgui.Display(static_cast<sf::RenderWindow&>(target));
}
