#pragma once

#include <SFGUI/SFGUI.hpp>

#include <libsqee/base.hpp>
#include <libsqee/extra/basicmenu.hpp>

namespace sqt {

class SceneGameMenus : public sqe::SceneMenu {
public:
    SceneGameMenus(sq::Application*);
};

class HandlerGameMenus : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event&);
};

}
