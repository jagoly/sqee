#pragma once

#include <libsqee/scene.hpp>
#include <libsqee/extra/basicmenu.hpp>

namespace sqt {

class SceneMainMenu : public sqe::SceneMenu {
public:
    SceneMainMenu(sq::Application* _app);

    sfg::Box::Ptr wMainVBox;
      sfg::Box::Ptr wHeaderHBox;
        sfg::Label::Ptr wTitleLabel;
      sfg::Box::Ptr wLevelsHBox;
        sfg::ScrolledWindow::Ptr wLevelsScrollWin;
          sfg::Box::Ptr wLevelListVBox;
            std::vector<sfg::Box::Ptr> levelButtonVec;
        sfg::Box::Ptr wLevelInfoVBox;
      sfg::Box::Ptr wFooterHBox;
        sfg::Label::Ptr wAuthorLabel;

    void reload_level_list(std::string dirPath);
    void start_game(std::string filePath);
};

}
