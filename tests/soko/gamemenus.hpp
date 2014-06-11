#pragma once

#include <SFGUI/SFGUI.hpp>

#include <libsqee/base.hpp>
#include <libsqee/extra/basicmenu.hpp>

namespace sqt {

class SceneGameMenus : public sqe::SceneMenu {
public:
    SceneGameMenus(sq::Application*);

    bool active;

    sfg::Box::Ptr wHBox;
      sfg::Separator::Ptr wHSepA;
      sfg::Box::Ptr wVBox;
        sfg::Separator::Ptr wVSepA;
        sfg::Window::Ptr wMenuWindow;
          sfg::Box::Ptr wContentHBox;
            sfg::Button::Ptr wRestartBtn;
            sfg::Button::Ptr wResumeBtn;
            sfg::Button::Ptr wQuitBtn;
        sfg::Separator::Ptr wVSepB;
      sfg::Separator::Ptr wHSepB;

    void activate();
    void deactivate();
    void restart();
    void quit();
};

class HandlerGameMenus : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event&);
};

}