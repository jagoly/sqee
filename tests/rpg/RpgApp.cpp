#include <SFML/Window/Event.hpp>

#include <sqee/scripts/ChaiScript.hpp>
#include <sqee/app/DebugOverlay.hpp>
#include <sqee/app/Resources.hpp>
#include <sqee/debug/Logging.hpp>

#include "main/MainScene.hpp"

#include "Options.hpp"
#include "RpgApp.hpp"

#include "messages.hpp"


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
    Options::get().Window_Size = OPTION_WindowSize;
    sq::Application::update_options();
}

bool RpgApp::handle(sf::Event _event) {
    if (sq::Application::handle(_event)) return true;

    auto notify = [this](int _value, const string& _message, vector<string> _options) {
        this->overlay->notify(_message + _options[_value], 6u); };

    static Options& options = Options::get();

    // need to move mbus into sq::Application...
    auto& mbus = static_cast<MainScene&>(*activeScenes.back()).get_mbus();

    if (_event.type == sf::Event::KeyPressed) {

        if (_event.key.code == sf::Keyboard::V) {
            OPTION_VerticalSync = !OPTION_VerticalSync;
            notify(OPTION_VerticalSync, "vsync set to ", {"OFF", "ON"});
            update_options(); return true; }

        if (_event.key.code == sf::Keyboard::F) {
            options.Shadows_Filter = !options.Shadows_Filter;
            notify(options.Shadows_Filter, "shadow filtering set to ", {"LOW", "HIGH"});
            update_options(); return true; }

        if (_event.key.code == sf::Keyboard::S) {
            options.Shadows_Large = !options.Shadows_Large;
            notify(options.Shadows_Large, "shadow resolution set to ", {"LOW", "HIGH"});
            update_options(); return true; }

        if (_event.key.code == sf::Keyboard::C) {
            options.Vignette_Enable = !options.Vignette_Enable;
            notify(options.Vignette_Enable, "vignette set to ", {"OFF", "ON"});
            update_options(); return true; }

        if (_event.key.code == sf::Keyboard::B) {
            options.Bloom_Enable = !options.Bloom_Enable;
            notify(options.Bloom_Enable, "bloom set to ", {"OFF", "ON"});
            update_options(); return true; }

        if (_event.key.code == sf::Keyboard::L) {
            options.Shafts_Quality = ++options.Shafts_Quality == 3 ? 0 : options.Shafts_Quality;
            notify(options.Shafts_Quality, "shafts set to ", {"OFF", "LOW", "HIGH"});
            update_options(); return true; }

        if (_event.key.code == sf::Keyboard::O) {
            options.SSAO_Quality = ++options.SSAO_Quality == 3 ? 0 : options.SSAO_Quality;
            notify(options.SSAO_Quality, "ssao set to ", {"OFF", "LOW", "HIGH"});
            update_options(); return true; }

        if (_event.key.code == sf::Keyboard::A) {
            options.FSAA_Quality = ++options.FSAA_Quality == 3 ? 0 : options.FSAA_Quality;
            notify(options.FSAA_Quality, "fsaa set to ", {"OFF", "FXAA", "SMAA"});
            update_options(); return true; }

        if (_event.key.code == sf::Keyboard::Num1) { mbus.send_message(msg::Debug_1()); return true; }
        if (_event.key.code == sf::Keyboard::Num2) { mbus.send_message(msg::Debug_2()); return true; }
        if (_event.key.code == sf::Keyboard::Num3) { mbus.send_message(msg::Debug_3()); return true; }
        if (_event.key.code == sf::Keyboard::Num4) { mbus.send_message(msg::Debug_4()); return true; }
    }

    return false;
}
