#include <sqee/app/Application.hpp>
#include <sqee/app/SettingsMap.hpp>

#include "MainScene.hpp"
#include "MainHandler.hpp"

using namespace sqt;

bool HandlerMain::handle(const sf::Event& _event) {
    auto scene = appBase->get_scene<SceneMain>("main");

    if (_event.type == sf::Event::KeyPressed) {
        if (_event.key.code == sf::Keyboard::S) {
            int crnt = settings->crnt<int>("shadQlty");
            if      (crnt == 0) settings->mod<int>("shadQlty", 1);
            else if (crnt == 1) settings->mod<int>("shadQlty", 2);
            else if (crnt == 2) settings->mod<int>("shadQlty", 0);
            settings->apply(); appBase->update(); return true;
        }
        if (_event.key.code == sf::Keyboard::F) {
            int crnt = settings->crnt<int>("shadFltr");
            if      (crnt == 0) settings->mod<int>("shadFltr", 1);
            else if (crnt == 1) settings->mod<int>("shadFltr", 2);
            else if (crnt == 2) settings->mod<int>("shadFltr", 0);
            settings->apply(); appBase->update(); return true;
        }
        if (_event.key.code == sf::Keyboard::O) {
            int crnt = settings->crnt<int>("ssaoMode");
            if      (crnt == 0) settings->mod<int>("ssaoMode", 1);
            else if (crnt == 1) settings->mod<int>("ssaoMode", 2);
            else if (crnt == 2) settings->mod<int>("ssaoMode", 0);
            settings->apply(); appBase->update(); return true;
        }
        if (_event.key.code == sf::Keyboard::B) {
            int crnt = settings->crnt<int>("hdrbMode");
            if      (crnt == 0) settings->mod<int>("hdrbMode", 1);
            else if (crnt == 1) settings->mod<int>("hdrbMode", 2);
            else if (crnt == 2) settings->mod<int>("hdrbMode", 0);
            settings->apply(); appBase->update(); return true;
        }
        if (_event.key.code == sf::Keyboard::A) {
            int crnt = settings->crnt<int>("fxaaMode");
            if      (crnt == 0) settings->mod<int>("fxaaMode", 1);
            else if (crnt == 1) settings->mod<int>("fxaaMode", 2);
            else if (crnt == 2) settings->mod<int>("fxaaMode", 0);
            settings->apply(); appBase->update(); return true;
        }
        if (_event.key.code == sf::Keyboard::C) {
            int crnt = settings->crnt<int>("vignMode");
            if      (crnt == 0) settings->mod<int>("vignMode", 1);
            else if (crnt == 1) settings->mod<int>("vignMode", 2);
            else if (crnt == 2) settings->mod<int>("vignMode", 0);
            settings->apply(); appBase->update(); return true;
        }
        if (_event.key.code == sf::Keyboard::V) {
            int crnt = settings->crnt<int>("framelimit");
            if      (crnt == 0) settings->mod<int>("framelimit", 1);
            else if (crnt == 1) settings->mod<int>("framelimit", 2);
            else if (crnt == 2) settings->mod<int>("framelimit", 0);
            settings->apply(); appBase->update(); return true;
        }
    }

    if (_event.type == sf::Event::MouseButtonPressed) {
        if (_event.mouseButton.button == sf::Mouse::Left) {
            if (settings->crnt<bool>("console_active")) return false;
            appBase->mouse_relatify(); scene->focused = true; return true;
        }
    }
    return false;
}
