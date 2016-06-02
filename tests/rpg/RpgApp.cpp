#include <SFML/Window/Event.hpp>

#include <sqee/scripts/ChaiScript.hpp>
#include <sqee/app/DebugOverlay.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/app/Resources.hpp>

#include "main/MainScene.hpp"
#include "RpgApp.hpp"

using namespace sqt;
namespace maths = sq::maths;

RpgApp::RpgApp() {
    activeScenes.emplace_back(new MainScene(*this));
}

void RpgApp::eval_test_init() {
    try { chaiEngine->eval_file(sq::static_path() + "test_init.chai"); }
    catch (chai::exception::eval_error& err) {
        sq::log_error(err.pretty_print());
        quit(1);
    }
}

void RpgApp::update_options() {
    options.WindowSize = OPTION_WindowSize;
    sq::Application::update_options();
}

bool RpgApp::handle(sf::Event _event) {
    if (sq::Application::handle(_event)) return true;

    auto notify = [this](int _value, const string& _message, vector<string> _options) {
        this->overlay->notify(_message + _options[_value], 6u); };

    if (_event.type == sf::Event::KeyPressed) {

        if (_event.key.code == sf::Keyboard::V) {
            OPTION_VerticalSync = !OPTION_VerticalSync;
            notify(OPTION_VerticalSync, "vsync set to ", {"OFF", "ON"});
            update_options(); return true; }

        if (_event.key.code == sf::Keyboard::F) {
            options.ShadowFilter = !options.ShadowFilter;
            notify(options.ShadowFilter, "shadow filtering set to ", {"LOW", "HIGH"});
            update_options(); return true; }

        if (_event.key.code == sf::Keyboard::S) {
            options.ShadowLarge = !options.ShadowLarge;
            notify(options.ShadowLarge, "shadow resolution set to ", {"LOW", "HIGH"});
            update_options(); return true; }

        if (_event.key.code == sf::Keyboard::C) {
            options.Vignette = !options.Vignette;
            notify(options.Vignette, "vignette set to ", {"OFF", "ON"});
            update_options(); return true; }

        if (_event.key.code == sf::Keyboard::B) {
            options.Bloom = !options.Bloom;
            notify(options.Bloom, "bloom set to ", {"OFF", "ON"});
            update_options(); return true; }

        if (_event.key.code == sf::Keyboard::L) {
            options.Shafts = ++options.Shafts == 3 ? 0 : options.Shafts;
            notify(options.Shafts, "shafts set to ", {"OFF", "LOW", "HIGH"});
            update_options(); return true; }

        if (_event.key.code == sf::Keyboard::O) {
            options.SSAO = ++options.SSAO == 3 ? 0 : options.SSAO;
            notify(options.Shafts, "ssao set to ", {"OFF", "LOW", "HIGH"});
            update_options(); return true; }

        if (_event.key.code == sf::Keyboard::A) {
            options.FSAA = ++options.FSAA == 3 ? 0 : options.FSAA;
            notify(options.Shafts, "fsaa set to ", {"OFF", "FXAA", "SMAA"});
            update_options(); return true; }
    }

    return false;
}
