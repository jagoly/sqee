#pragma once

#include <SFGUI/SFGUI.hpp>

#include <libsqee/base.hpp>
#include <libsqee/extra/basicmenu.hpp>

namespace sqt {

class SceneMenus : public sqe::SceneMenu {
public:
    SceneMenus(sq::Application*);

    sfg::Box::Ptr wHBox;
};

class HandlerMenus : public sqe::HandlerMenu {
public:
    using sqe::HandlerMenu::HandlerMenu;
    bool handle(sf::Event&);
};

}
