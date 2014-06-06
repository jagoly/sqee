#pragma once

#include <SFGUI/SFGUI.hpp>

#include <libsqee/base.hpp>

namespace sqt {

class SceneMainMenu : public sq::Scene {
public:
    SceneMainMenu(sq::Application*);

    sfg::SFGUI sfgui;
    sfg::Desktop desktop;
    sfg::Window::Ptr window;

    sfg::Box::Ptr wMainVBox;
      sfg::Box::Ptr wHeaderHBox;
        sfg::Separator::Ptr wHeaderSepLeft;
        sfg::Label::Ptr wTitleLabel;
        sfg::Separator::Ptr wHeaderSepRight;
      sfg::Box::Ptr wLevelsHBox;
        sfg::ScrolledWindow::Ptr wLevelsScrollWin;
          sfg::Box::Ptr wLevelListVBox;
            std::vector<sfg::Box::Ptr> levelButtonVec;
        sfg::Box::Ptr wLevelInfoVBox;
      sfg::Box::Ptr wFooterHBox;
        sfg::Separator::Ptr wFooterSep;
        sfg::Label::Ptr wAuthorLabel;

    void reload_level_list(std::string);
    void start_game(std::string);

    void update();
    void render(sf::RenderTarget&, float);
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event&);
};

}
