#include <SFML/Window/Event.hpp>

#include <sqee/app/Application.hpp>
#include <sqee/app/Settings.hpp>

#include "MainScene.hpp"
#include "MainHandler.hpp"

using namespace sqt;

bool MainHandler::handle(sf::Event _event) {
    MainScene& scene = app->get_scene<MainScene>("main");

    if (_event.type == sf::Event::KeyPressed) {
        if (_event.key.code == sf::Keyboard::F) {
            bool val = app->settings.get<bool>("rpg_shadfilter");
            app->overlay.notify(string("shadow filtering set to ")
                                + (val ? "LOW" : "HIGH"), 6u);
            app->settings.mod<bool>("rpg_shadfilter", !val);
            app->refresh(); return true;
        }
        if (_event.key.code == sf::Keyboard::S) {
            bool val = app->settings.get<bool>("rpg_shadlarge");
            app->overlay.notify(string("shadow resolution set to ")
                                + (val ? "SMALL" : "LARGE"), 6u);
            app->settings.mod<bool>("rpg_shadlarge", !val);
            app->refresh(); return true;
        }
        if (_event.key.code == sf::Keyboard::C) {
            bool val = app->settings.get<bool>("rpg_vignette");
            app->overlay.notify(string("vignetting set to ")
                                + (val ? "OFF" : "ON"), 6u);
            app->settings.mod<bool>("rpg_vignette", !val);
            app->refresh(); return true;
        }
        if (_event.key.code == sf::Keyboard::B) {
            bool val = app->settings.get<bool>("rpg_bloom");
            app->overlay.notify(string("hdr bloom set to ")
                                + (val ? "OFF" : "ON"), 6u);
            app->settings.mod<bool>("rpg_bloom", !val);
            app->refresh(); return true;
        }
        if (_event.key.code == sf::Keyboard::I) {
            bool val = app->settings.get<bool>("rpg_irrd");
            app->settings.mod<bool>("rpg_irrd", !val);
            app->refresh(); return true;
        }
        if (_event.key.code == sf::Keyboard::L) {
            int val = app->settings.get<int>("rpg_shafts");
            app->overlay.notify(string("light shafts set to ") +
                                (val ? (val>1 ? "OFF" : "HIGH") : "LOW"), 6u);
            app->settings.mod<int>("rpg_shafts", ++val == 3 ? 0 : val);
            app->refresh(); return true;
        }
        if (_event.key.code == sf::Keyboard::O) {
            int val = app->settings.get<int>("rpg_ssao");
            app->overlay.notify(string("ssao set to ") +
                                (val ? (val>1 ? "OFF" : "HIGH") : "LOW"), 6u);
            app->settings.mod<int>("rpg_ssao", ++val == 3 ? 0 : val);
            app->refresh(); return true;
        }
        if (_event.key.code == sf::Keyboard::A) {
            int val = app->settings.get<int>("rpg_fxaa");
            app->overlay.notify(string("anti-aliasing set to ") +
                                (val ? (val>1 ? "OFF" : "SMAA") : "FXAA"), 6u);
            app->settings.mod<int>("rpg_fxaa", ++val == 3 ? 0 : val);
            app->refresh(); return true;
        }
        if (_event.key.code == sf::Keyboard::V) {
            int val = app->settings.get<int>("app_fpslimit");
            app->overlay.notify(string("framerate limit set to ") +
                                (val ? (val>1 ? "NONE" : "VSYNC") : "75FPS"), 6u);
            app->settings.mod<int>("app_fpslimit", ++val == 3 ? 0 : val);
            app->refresh(); return true;
        }
    }

//    if (_event.type == sf::Event::MouseButtonPressed) {
//        if (_event.mouseButton.button == sf::Mouse::Left) {
//            if (app->settings.get<bool>("console_active")) return false;
//            app->mouse_relatify(); scene->focused = true; return true;
//        }
//    }
    return false;
}
