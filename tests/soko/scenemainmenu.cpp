#include <boost/filesystem.hpp>

#include <libsqee/extra/helpers.hpp>

#include "scenemainmenu.hpp"
#include "scenemain.hpp"
#include "scenehud.hpp"

using namespace sqt;
namespace fs = boost::filesystem;

SceneMainMenu::SceneMainMenu(sq::Application* _app) : sq::Scene(_app) {
    desktop.SetProperty("Label#titlelabel", "FontSize", 32);

    desktop.SetProperty("Separator.blanksep", "Color", sf::Color::Transparent);

    desktop.SetProperty("Button.levelbutton", "FontSize", 20);
    desktop.SetProperty("Button.levelbutton", "BackgroundColor", sf::Color({80, 40, 90}));
    desktop.SetProperty("Button.levelbutton:PRELIGHT", "BackgroundColor", sf::Color({85, 50, 95}));
    desktop.SetProperty("Button.levelbutton:ACTIVE", "BackgroundColor", sf::Color({50, 20, 60}));
    desktop.SetProperty("Button.levelbutton:ACTIVE", "Color", sf::Color::White);

    window = sfg::Window::Create(sfg::Window::Style::NO_STYLE);

    wMainVBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
      wHeaderHBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
        wHeaderSepLeft = sfg::Separator::Create(sfg::Separator::Orientation::HORIZONTAL);
        wHeaderSepLeft->SetClass("blanksep");
        wTitleLabel = sfg::Label::Create("Sokoban Test (SQEE)");
        wTitleLabel->SetId("titlelabel");
        wHeaderSepRight = sfg::Separator::Create(sfg::Separator::Orientation::HORIZONTAL);
        wHeaderSepRight->SetClass("blanksep");
      wLevelsHBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
        wLevelsScrollWin = sfg::ScrolledWindow::Create();
        wLevelsScrollWin->SetScrollbarPolicy(sfg::ScrolledWindow::HORIZONTAL_NEVER | sfg::ScrolledWindow::VERTICAL_ALWAYS);
          wLevelListVBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
        wLevelInfoVBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
      wFooterHBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
        wFooterSep = sfg::Separator::Create(sfg::Separator::Orientation::HORIZONTAL);
        wFooterSep->SetClass("blanksep");
        wAuthorLabel = sfg::Label::Create("Jagoly and the SQEE Team");
        wAuthorLabel->SetId("authorlabel");

    wMainVBox->Pack(wHeaderHBox, false);
      wHeaderHBox->Pack(wHeaderSepLeft);
      wHeaderHBox->Pack(wTitleLabel, false);
      wHeaderHBox->Pack(wHeaderSepRight);
    wMainVBox->Pack(wLevelsHBox);
      wLevelsHBox->Pack(wLevelsScrollWin, false);
        wLevelsScrollWin->AddWithViewport(wLevelListVBox);
      wLevelsHBox->Pack(wLevelInfoVBox);
    wMainVBox->Pack(wFooterHBox, false);
      wFooterHBox->Pack(wFooterSep);
      wFooterHBox->Pack(wAuthorLabel, false);

    window->Add(wMainVBox);
    desktop.Add(window);

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
    app->attach_handler("main", std::shared_ptr<sq::Handler>(new HandlerGame(app)));
    app->prepend_scene("main", std::shared_ptr<sq::Scene>(new SceneMain(app)));

    Level level(filePath);
    static_cast<SceneMain*>(&app->get_scene("main"))->load_level(level);

    app->sweep_handler("mainmenu");
    app->sweep_scene("mainmenu");
}

void SceneMainMenu::update() {
    window->SetAllocation({0, 0,
                           static_cast<float>(app->window->getSize().x),
                           static_cast<float>(app->window->getSize().y)});
}

void SceneMainMenu::render(sf::RenderTarget& target, float ft) {
    desktop.Update(ft);
    sfgui.Display(static_cast<sf::RenderWindow&>(target));
}

bool HandlerGame::handle(sf::Event& event) {
    return false;
}
