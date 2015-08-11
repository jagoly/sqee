#include <sqee/app/Application.hpp>
#include <sqee/app/Settings.hpp>

#include "MainScene.hpp"
#include "MainHandler.hpp"

using namespace sqt;

bool MainHandler::handle(const sf::Event& _event) {
    auto scene = appBase->get_scene<MainScene>("main");

    if (_event.type == sf::Event::KeyPressed) {
        if (_event.key.code == sf::Keyboard::F) {
            bool val = settings->get<bool>("rpg_shadfilter");
            settings->mod<bool>("rpg_shadfilter", !val);
            appBase->update(); return true;
        }
        if (_event.key.code == sf::Keyboard::S) {
            bool val = settings->get<bool>("rpg_shadlarge");
            settings->mod<bool>("rpg_shadlarge", !val);
            appBase->update(); return true;
        }
        if (_event.key.code == sf::Keyboard::C) {
            bool val = settings->get<bool>("rpg_vignette");
            settings->mod<bool>("rpg_vignette", !val);
            appBase->update(); return true;
        }
        if (_event.key.code == sf::Keyboard::B) {
            bool val = settings->get<bool>("rpg_bloom");
            settings->mod<bool>("rpg_bloom", !val);
            appBase->update(); return true;
        }
        if (_event.key.code == sf::Keyboard::L) {
            int val = settings->get<int>("rpg_shafts");
            settings->mod<int>("rpg_shafts", ++val == 3 ? 0 : val);
            appBase->update(); return true;
        }
        if (_event.key.code == sf::Keyboard::O) {
            int val = settings->get<int>("rpg_ssao");
            settings->mod<int>("rpg_ssao", ++val == 3 ? 0 : val);
            appBase->update(); return true;
        }
        if (_event.key.code == sf::Keyboard::A) {
            int val = settings->get<int>("rpg_fxaa");
            settings->mod<int>("rpg_fxaa", ++val == 3 ? 0 : val);
            appBase->update(); return true;
        }
        if (_event.key.code == sf::Keyboard::V) {
            int val = settings->get<int>("framelimit");
            settings->mod<int>("framelimit", ++val == 3 ? 0 : val);
            appBase->update(); return true;
        }
    }

    if (_event.type == sf::Event::MouseButtonPressed) {
        if (_event.mouseButton.button == sf::Mouse::Left) {
            if (settings->get<bool>("console_active")) return false;
            appBase->mouse_relatify(); scene->focused = true; return true;
        }
    }
    return false;
}
