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
        sfg::Box::Ptr wLevelListVBox;
          sfg::Separator::Ptr wLevelListSep;
      sfg::Box::Ptr wFooterHBox;
        sfg::Separator::Ptr wFooterSep;
        sfg::Label::Ptr wAuthorLabel;

    void update();
    void render(sf::RenderTarget&, float);
};

}
