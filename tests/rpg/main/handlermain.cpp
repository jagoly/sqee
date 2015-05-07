#include <sqee/app/application.hpp>

#include "scenemain.hpp"
#include "handlermain.hpp"

using namespace sqt;

bool HandlerMain::handle(const sf::Event& _event) {
    //SceneMain& sMain = *static_cast<SceneMain*>(app.sceneIM.get("main").get());

    if (_event.type == sf::Event::KeyPressed) {
        if (_event.key.code == sf::Keyboard::S) {
            int crnt = app.settings.crnt<int>("shadQlty");
            if      (crnt == 0) app.settings.mod<int>("shadQlty", 1);
            else if (crnt == 1) app.settings.mod<int>("shadQlty", 2);
            else if (crnt == 2) app.settings.mod<int>("shadQlty", 0);
            app.settings.apply();
            return true;
        }
        if (_event.key.code == sf::Keyboard::F) {
            int crnt = app.settings.crnt<int>("shadFltr");
            if      (crnt == 0) app.settings.mod<int>("shadFltr", 1);
            else if (crnt == 1) app.settings.mod<int>("shadFltr", 2);
            else if (crnt == 2) app.settings.mod<int>("shadFltr", 0);
            app.settings.apply();
            return true;
        }
        if (_event.key.code == sf::Keyboard::O) {
            int crnt = app.settings.crnt<int>("ssaoQlty");
            if      (crnt == 0) app.settings.mod<int>("ssaoQlty", 1);
            else if (crnt == 1) app.settings.mod<int>("ssaoQlty", 2);
            else if (crnt == 2) app.settings.mod<int>("ssaoQlty", 0);
            app.settings.apply();
            return true;
        }
        if (_event.key.code == sf::Keyboard::B) {
            int crnt = app.settings.crnt<int>("hdrbQlty");
            if      (crnt == 0) app.settings.mod<int>("hdrbQlty", 1);
            else if (crnt == 1) app.settings.mod<int>("hdrbQlty", 2);
            else if (crnt == 2) app.settings.mod<int>("hdrbQlty", 0);
            app.settings.apply();
            return true;
        }
        if (_event.key.code == sf::Keyboard::A) {
            int crnt = app.settings.crnt<int>("fxaaQlty");
            if      (crnt == 0) app.settings.mod<int>("fxaaQlty", 1);
            else if (crnt == 1) app.settings.mod<int>("fxaaQlty", 2);
            else if (crnt == 2) app.settings.mod<int>("fxaaQlty", 0);
            app.settings.apply();
            return true;
        }
        if (_event.key.code == sf::Keyboard::C) {
            bool crnt = app.settings.crnt<bool>("vignetting");
            app.settings.mod<bool>("vignetting", !crnt);
            app.settings.apply();
            return true;
        }
        if (_event.key.code == sf::Keyboard::V) {
            bool crnt = app.settings.crnt<bool>("app_vsync");
            app.settings.mod<bool>("app_vsync", !crnt);
            app.settings.apply();
            return true;
        }
        return false;
    }

    if (_event.type == sf::Event::MouseButtonPressed) {
        if (_event.mouseButton.button == sf::Mouse::Left) {
            bool crnt = app.settings.crnt<bool>("mouseFocus");
            app.settings.mod<bool>("mouseFocus", !crnt);
            app.settings.apply();
            return true;
        }
    }
    return false;
}
