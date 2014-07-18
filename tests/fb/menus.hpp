#pragma once

#include <libsqee/base.hpp>
#include <libsqee/extra/basicmenu.hpp>

namespace sqt {

class SceneMenus : public sqe::SceneMenu {
public:
    SceneMenus(sq::Application*);

    bool gameActive;
    bool showScore;

    sfg::Box::Ptr wHBox;

    void render(sf::RenderTarget&, float);
};

class HandlerMenus : public sqe::HandlerMenu {
public:
    using sqe::HandlerMenu::HandlerMenu;
    bool handle(sf::Event&);
};

}
