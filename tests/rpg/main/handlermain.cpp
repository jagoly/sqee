#include <sqee/app/application.hpp>
#include <sqee/app/settings.hpp>

#include "scenemain.hpp"
#include "handlermain.hpp"

using namespace sqt;

bool HandlerMain::handle(const sf::Event& _event) {
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
            bool crnt = settings->crnt<bool>("app_vsync");
            settings->mod<bool>("app_vsync", !crnt);
            settings->apply(); appBase->update(); return true;
        }
    }

    if (_event.type == sf::Event::MouseButtonPressed) {
        if (_event.mouseButton.button == sf::Mouse::Left) {
            bool crnt = settings->crnt<bool>("mouseFocus");
            settings->mod<bool>("mouseFocus", !crnt);
            settings->apply(); appBase->update(); return true;
        }
    }
    return false;
}
