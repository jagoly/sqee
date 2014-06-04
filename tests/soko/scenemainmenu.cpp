#include "scenemainmenu.hpp"

using namespace sqt;

SceneMainMenu::SceneMainMenu(sq::Application* _app) : sq::Scene(_app) {
    desktop.SetProperty("Label#titlelabel", "FontSize", 32);
    desktop.SetProperty("Separator.blanksep", "Color", sf::Color::Transparent);

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
        wLevelListVBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
          wLevelListSep = sfg::Separator::Create(sfg::Separator::Orientation::VERTICAL);
          wLevelListSep->SetClass("blanksep");
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
      wLevelsHBox->Pack(wLevelListVBox, false);
        // list levels
        wLevelListVBox->Pack(wLevelListSep);
    wMainVBox->Pack(wFooterHBox, false);
      wFooterHBox->Pack(wFooterSep);
      wFooterHBox->Pack(wAuthorLabel, false);

    window->Add(wMainVBox);
    desktop.Add(window);
}

void SceneMainMenu::update() {
    window->SetAllocation({0, 0, app->window->getSize().x, app->window->getSize().y});
}

void SceneMainMenu::render(sf::RenderTarget& target, float ft) {
    desktop.Update(ft);
    sfgui.Display(static_cast<sf::RenderWindow&>(target));
}
