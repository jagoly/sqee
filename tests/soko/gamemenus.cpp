#include "gamemenus.hpp"
#include "scenegame.hpp"
#include "mainmenu.hpp"

namespace sqt {

SceneGameMenus::SceneGameMenus(sq::Application* _app) : sqe::SceneMenu(_app, false) {
    desktop.SetProperty("Separator", "Color", sf::Color::Transparent);
    desktop.SetProperty("Button", "BackgroundColor", sf::Color(30, 30, 30, 60));
    desktop.SetProperty("Button:PRELIGHT", "BackgroundColor", sf::Color(50, 50, 50, 60));
    desktop.SetProperty("Button:ACTIVE", "BackgroundColor", sf::Color(10, 10, 10, 60));
    desktop.SetProperty("Window#menu", "BackgroundColor", sf::Color(50, 20, 60, 224));

    wHBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
      wHSepA = sfg::Separator::Create(sfg::Separator::Orientation::HORIZONTAL);
      wHSepA->SetClass("blank");
      wVBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
        wVSepA = sfg::Separator::Create(sfg::Separator::Orientation::VERTICAL);
        wMenuWindow = sfg::Window::Create(sfg::Window::Style::BACKGROUND);
        wMenuWindow->SetId("menu");
        wMenuWindow->SetRequisition({600.f, 450.f});
          wContentHBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
          wContentHBox->SetSpacing(10.f);
            wResumeBtn = sfg::Button::Create("Resume");
            wResumeBtn->SetRequisition({186.f, 0.f});
            wResumeBtn->GetSignal(sfg::Button::OnLeftClick).Connect(
                                  std::bind(&sqt::SceneGameMenus::deactivate, this));
            wRestartBtn = sfg::Button::Create("Restart");
            wRestartBtn->SetRequisition({186.f, 0.f});
            wRestartBtn->GetSignal(sfg::Button::OnLeftClick).Connect(
                                   std::bind(&sqt::SceneGameMenus::restart, this));
            wQuitBtn = sfg::Button::Create("Quit");
            wQuitBtn->SetRequisition({186.f, 0.f});
            wQuitBtn->GetSignal(sfg::Button::OnLeftClick).Connect(
                                std::bind(&sqt::SceneGameMenus::quit, this));
        wVSepB = sfg::Separator::Create(sfg::Separator::Orientation::VERTICAL);
        wVSepB->SetClass("blank");
      wHSepB = sfg::Separator::Create(sfg::Separator::Orientation::HORIZONTAL);
      wHSepB->SetClass("blank");

    wHBox->Pack(wHSepA);
    wHBox->Pack(wVBox, false);
      wVBox->Pack(wVSepA);
      wVBox->Pack(wMenuWindow, false);
        wMenuWindow->Add(wContentHBox);
          wContentHBox->Pack(wResumeBtn);
          wContentHBox->Pack(wRestartBtn);
          wContentHBox->Pack(wQuitBtn);
      wVBox->Pack(wVSepB);
    wHBox->Pack(wHSepB);

    rootWindow->Add(wHBox);
}

void SceneGameMenus::activate() {
    sqe::SceneMenu::activate();
    static_cast<SceneGame&>(app->get_scene("game")).active = false;
}

void SceneGameMenus::deactivate() {
    sqe::SceneMenu::deactivate();
    static_cast<SceneGame&>(app->get_scene("game")).active = true;
}

void SceneGameMenus::restart() {
    static_cast<SceneGame&>(app->get_scene("game")).start_level();
    deactivate();
}

void SceneGameMenus::quit() {
    app->attach_handler("mainmenu", sq::HandlerPtr(new sqe::HandlerMenu(app, "mainmenu")));
    app->prepend_scene("mainmenu", sq::ScenePtr(new SceneMainMenu(app)));

    app->sweep_handler("gamemenus");
    app->sweep_scene("gamemenus");
    app->sweep_scene("game");
}

bool HandlerGameMenus::handle(sf::Event& event) {
    bool bRet = sqe::HandlerMenu::handle(event);
    if (bRet) return true;
    if (event.type == sf::Event::LostFocus ||
        (SQ_KEYPRESS(Escape))) {
        static_cast<SceneGameMenus&>(app->get_scene(menuId)).activate();
        return true;
    }
    return false;
}

}
