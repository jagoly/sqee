#pragma once

#include <libsqee/scene.hpp>
#include <libsqee/extra/basicmenu.hpp>

namespace sqt {

class SceneMenus : public sq::SceneMenu {
public:
    SceneMenus(sq::Application* _app);

    bool gameActive;
    bool showScore;

    sfg::Box::Ptr wHBox;

    void render(sf::RenderTarget& target, float ft);
};

class HandlerMenus : public sq::HandlerMenu {
public:
    using sq::HandlerMenu::HandlerMenu;
    bool handle(sf::Event& event);
};

}
