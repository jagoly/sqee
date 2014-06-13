#include <boost/filesystem.hpp>

#include <libsqee/extra/helpers.hpp>

#include "mainmenu.hpp"
#include "gamemenus.hpp"
#include "scenegame.hpp"

using namespace sqt;
namespace fs = boost::filesystem;

SceneMainMenu::SceneMainMenu(sq::Application* _app) : sqe::SceneMenu(_app, true) {
    desktop.SetProperty("Label#titlelabel", "FontSize", 32);

    desktop.SetProperty("Button.levelbutton", "FontSize", 20);
    desktop.SetProperty("Button.levelbutton", "BackgroundColor", sf::Color({80, 40, 90}));
    desktop.SetProperty("Button.levelbutton:PRELIGHT", "BackgroundColor", sf::Color({85, 50, 95}));
    desktop.SetProperty("Button.levelbutton:ACTIVE", "BackgroundColor", sf::Color({50, 20, 60}));
    desktop.SetProperty("Button.levelbutton:ACTIVE", "Color", sf::Color::White);

    wMainVBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
      wHeaderHBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
        wTitleLabel = sfg::Label::Create("Sokoban Test (SQEE)");
        wTitleLabel->SetId("titlelabel");
        wTitleLabel->SetAlignment({0.5f, 0.f});
      wLevelsHBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
        wLevelsScrollWin = sfg::ScrolledWindow::Create();
        wLevelsScrollWin->SetScrollbarPolicy(sfg::ScrolledWindow::HORIZONTAL_NEVER | sfg::ScrolledWindow::VERTICAL_ALWAYS);
          wLevelListVBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
        wLevelInfoVBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
      wFooterHBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
        wAuthorLabel = sfg::Label::Create("Jagoly and the SQEE Team");
        wAuthorLabel->SetId("authorlabel");
        wAuthorLabel->SetAlignment({1.f, 0.f});

    wMainVBox->Pack(wHeaderHBox, false);
      wHeaderHBox->Pack(wTitleLabel);
    wMainVBox->Pack(wLevelsHBox);
      wLevelsHBox->Pack(wLevelsScrollWin, false);
        wLevelsScrollWin->AddWithViewport(wLevelListVBox);
      wLevelsHBox->Pack(wLevelInfoVBox);
    wMainVBox->Pack(wFooterHBox, false);
      wFooterHBox->Pack(wAuthorLabel);

    rootWindow->Add(wMainVBox);

    reload_level_list("res/levels");
}

void SceneMainMenu::reload_level_list(std::string dirPath) {
    std::vector<std::string> pathVec;
    for (fs::directory_iterator it(dirPath); it != fs::directory_iterator(); ++it) {
        pathVec.push_back(it->path().string());
    }
    std::sort(std::begin(pathVec), std::end(pathVec));

    for (auto& path : pathVec) {
        auto root = sqe::load_json_file(path);

        auto btn = sfg::Button::Create(root["name"].asString());\
        btn->SetClass("levelbutton");
        btn->GetSignal(sfg::Button::OnLeftClick).Connect( [this, path] {
            start_game(path);
        });

        wLevelListVBox->Pack(btn);
    }
}

void SceneMainMenu::start_game(std::string filePath) {
    app->attach_handler("gamemenus", sq::HandlerPtr(new HandlerGameMenus(app, "gamemenus")));
    app->prepend_scene("gamemenus", sq::ScenePtr(new SceneGameMenus(app)));
    app->prepend_scene("game", sq::ScenePtr(new SceneGame(app)));

    static_cast<SceneGame*>(&app->get_scene("game"))->load_level(filePath);
    static_cast<SceneGame*>(&app->get_scene("game"))->start_level();

    app->sweep_handler("mainmenu");
    app->sweep_scene("mainmenu");
}
