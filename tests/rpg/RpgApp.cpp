#include <SFML/Window/Event.hpp>

#include <sqee/scripts/ChaiEngine.hpp>
#include <sqee/app/DebugOverlay.hpp>
#include <sqee/debug/Logging.hpp>

#include "main/MainScene.hpp"

#include "Options.hpp"
#include "messages.hpp"
#include "RpgApp.hpp"

using namespace sqt;
namespace maths = sq::maths;

//============================================================================//

RpgApp::RpgApp()
{
    mScenes.emplace_back(new MainScene(*this));
}

//============================================================================//

void RpgApp::eval_test_init()
{
    try { mChaiEngine->eval_file("assets/test_init.chai"); }
    catch (chai::exception::eval_error& error)
    { sq::log_error(error.pretty_print()); }
}

//============================================================================//

void RpgApp::update_options()
{
    Options::get().Window_Size = OPTION_WindowSize;
    sq::Application::update_options();
}

//============================================================================//

bool RpgApp::handle(sf::Event event)
{
    if (sq::Application::handle(event)) return true;

    auto notify = [this](uint value, const string& message, vector<string> options)
    { this->mDebugOverlay->notify(message + options[value], 6u); };

    static Options& options = Options::get();

    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::V)
        {
            OPTION_VerticalSync = !OPTION_VerticalSync;
            notify(OPTION_VerticalSync, "vsync set to ", {"OFF", "ON"});
            update_options(); return true;
        }

        if (event.key.code == sf::Keyboard::F)
        {
            options.Shadows_Filter = !options.Shadows_Filter;
            notify(options.Shadows_Filter, "shadow filtering set to ", {"LOW", "HIGH"});
            update_options(); return true;
        }

        if (event.key.code == sf::Keyboard::S)
        {
            options.Shadows_Large = !options.Shadows_Large;
            notify(options.Shadows_Large, "shadow resolution set to ", {"LOW", "HIGH"});
            update_options(); return true;
        }

        if (event.key.code == sf::Keyboard::C)
        {
            options.Vignette_Enable = !options.Vignette_Enable;
            notify(options.Vignette_Enable, "vignette set to ", {"OFF", "ON"});
            update_options(); return true;
        }

        if (event.key.code == sf::Keyboard::B)
        {
            options.Bloom_Enable = !options.Bloom_Enable;
            notify(options.Bloom_Enable, "bloom set to ", {"OFF", "ON"});
            update_options(); return true;
        }

        if (event.key.code == sf::Keyboard::L)
        {
            options.Shafts_Quality = ++options.Shafts_Quality == 3 ? 0 : options.Shafts_Quality;
            notify(options.Shafts_Quality, "shafts set to ", {"OFF", "LOW", "HIGH"});
            update_options(); return true;
        }

        if (event.key.code == sf::Keyboard::O)
        {
            options.SSAO_Quality = ++options.SSAO_Quality == 3 ? 0 : options.SSAO_Quality;
            notify(options.SSAO_Quality, "ssao set to ", {"OFF", "LOW", "HIGH"});
            update_options(); return true;
        }

        if (event.key.code == sf::Keyboard::A)
        {
            options.FSAA_Quality = ++options.FSAA_Quality == 3 ? 0 : options.FSAA_Quality;
            notify(options.FSAA_Quality, "fsaa set to ", {"OFF", "FXAA", "SMAA"});
            update_options(); return true;
        }

        if (event.key.code == sf::Keyboard::Num1) { mMessageBus->send_message(msg::Debug_1()); return true; }
        if (event.key.code == sf::Keyboard::Num2) { mMessageBus->send_message(msg::Debug_2()); return true; }
        if (event.key.code == sf::Keyboard::Num3) { mMessageBus->send_message(msg::Debug_3()); return true; }
        if (event.key.code == sf::Keyboard::Num4) { mMessageBus->send_message(msg::Debug_4()); return true; }
    }

    return false;
}
