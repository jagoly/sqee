#include "RpgApp.hpp"

#include "main/Scripting.hpp"

#include <sqee/app/Event.hpp>
#include <sqee/app/LuaSetup.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/debug/Misc.hpp>

using namespace sqt;

//============================================================================//

RpgApp::RpgApp() = default;

RpgApp::~RpgApp() = default;

//============================================================================//

void RpgApp::initialise(std::vector<std::string> args)
{
    (void) args;

    //--------------------------------------------------------//

    mWrenVM = std::make_unique<wren::WrenPlusVM>();

    mWindow = std::make_unique<sq::Window>("SQEE RPG", Vec2U(1280, 720));

    mInputDevices = std::make_unique<sq::InputDevices>(*mWindow);

    mWrenConsole = std::make_unique<WrenConsole>(*mWrenVM);

    mDebugOverlay = std::make_unique<sq::DebugOverlay>();

    //--------------------------------------------------------//

    mResourceCaches = std::make_unique<ResourceCaches>();

    mMainScene = std::make_unique<MainScene>(mOptions, *mWrenVM, *mInputDevices, *mResourceCaches);

    //--------------------------------------------------------//

    mWindow->set_cursor_hidden(true);

    sq::wren_setup_maths(*mWrenVM);

    //--------------------------------------------------------//

//    mChaiEngine->add(chai::fun(&sq::ChaiConsole::clear, mChaiConsole.get()), "console_clear");
//    mChaiEngine->add(chai::fun(&sq::ChaiConsole::history, mChaiConsole.get()), "console_history");
//    mChaiEngine->add(chai::fun(&sq::ChaiConsole::print, mChaiConsole.get()), "console_print");
//    mChaiEngine->add(chai::fun(&sq::DebugOverlay::notify, mDebugOverlay.get()), "overlay_notify");

    //--------------------------------------------------------//

    wren_setup_world(*mWrenVM);
    wren_setup_api(*mWrenVM);

    //--------------------------------------------------------//

    this->refresh_options();

//    try { mChaiEngine->eval_file("assets/test_init.chai"); }
//    catch (const chai::exception::eval_error& error)
//    { sq::log_error(error.pretty_print()); }
}

void RpgApp::update(double elapsed)
{
    //-- fetch and handle events -----------------------------//

    for (const auto& event : mWindow->fetch_events())
        handle_event(event);

    //-- update and render the main scene --------------------//

    mMainScene->update_and_render(elapsed);

    //-- update and render the console and overlay -----------//

    mWrenConsole->update_and_render(elapsed);
    mDebugOverlay->update_and_render(elapsed);

    //-- swap da bufaz ---------------------------------------//

    mWindow->swap_buffers();
}

//============================================================================//

void RpgApp::refresh_options()
{
    mOptions.Window_Size = mWindow->get_window_size();

    mMainScene->refresh_options();
}

//============================================================================//

void RpgApp::handle_event(sq::Event event)
{
    using Type = sq::Event::Type;
    using Key = sq::Keyboard_Key;

    const auto& [type, data] = event;

    //--------------------------------------------------------//

    if (type == Type::Window_Close)
    {
        mReturnCode = 0;
        return;
    }

    //--------------------------------------------------------//

    if (type == Type::Window_Resize)
    {
        refresh_options();
        return;
    }

    //--------------------------------------------------------//

    if (type == Type::Keyboard_Press && data.keyboard.key == Key::Menu)
    {
        if (event.data.keyboard.ctrl == true)
        {
            mWindow->set_cursor_hidden(mWrenConsole->check_active());
            mMainScene->set_input_enabled(mWrenConsole->check_active());

            mWrenConsole->toggle_active();
        }

        else mDebugOverlay->toggle_active();

        return;
    }

    //--------------------------------------------------------//

    if (mWrenConsole->check_active() == true)
    {
        if ( type == Type::Keyboard_Press || type == Type::Keyboard_Release ||
             type == Type::Mouse_Press || type == Type::Mouse_Release ||
             type == Type::Text_Entry )
        {
            mWrenConsole->handle_event(event);
        }

        return; // munch events
    }

    //--------------------------------------------------------//

    auto notify = [this](uint value, const std::string& message, std::vector<std::string> options)
    { this->mDebugOverlay->notify(message + options[value]); };

    if (type == Type::Keyboard_Press)
    {
        if (data.keyboard.key == Key::V)
        {
            auto value = !mWindow->get_vsync_enabled();
            mWindow->set_vsync_enabled(value);
            notify(value, "vsync set to ", {"OFF", "ON"});
        }

        if (data.keyboard.key == Key::F)
        {
            mOptions.Shadows_Filter = !mOptions.Shadows_Filter;
            notify(mOptions.Shadows_Filter, "shadow filtering set to ", {"LOW", "HIGH"});
            refresh_options();
        }

        if (data.keyboard.key == Key::S)
        {
            mOptions.Shadows_Large = !mOptions.Shadows_Large;
            notify(mOptions.Shadows_Large, "shadow resolution set to ", {"LOW", "HIGH"});
            refresh_options();
        }

        if (data.keyboard.key == Key::C)
        {
            mOptions.Vignette_Enable = !mOptions.Vignette_Enable;
            notify(mOptions.Vignette_Enable, "vignette set to ", {"OFF", "ON"});
            refresh_options();
        }

        if (data.keyboard.key == Key::B)
        {
            mOptions.Bloom_Enable = !mOptions.Bloom_Enable;
            notify(mOptions.Bloom_Enable, "bloom set to ", {"OFF", "ON"});
            refresh_options();
        }

        if (data.keyboard.key == Key::L)
        {
            mOptions.Shafts_Quality = ++mOptions.Shafts_Quality == 3 ? 0 : mOptions.Shafts_Quality;
            notify(mOptions.Shafts_Quality, "shafts set to ", {"OFF", "LOW", "HIGH"});
            refresh_options();
        }

        if (data.keyboard.key == Key::O)
        {
            mOptions.SSAO_Quality = ++mOptions.SSAO_Quality == 3 ? 0 : mOptions.SSAO_Quality;
            notify(mOptions.SSAO_Quality, "ssao set to ", {"OFF", "LOW", "HIGH"});
            refresh_options();
        }

        if (data.keyboard.key == Key::A)
        {
            mOptions.FSAA_Quality = ++mOptions.FSAA_Quality == 3 ? 0 : mOptions.FSAA_Quality;
            notify(mOptions.FSAA_Quality, "fsaa set to ", {"OFF", "FXAA", "SMAA"});
            refresh_options();
        }

        if (data.keyboard.key == Key::D)
        {
            if      ( mOptions.Debug_Texture == ""           ) mOptions.Debug_Texture = "diffuse";
            else if ( mOptions.Debug_Texture == "diffuse"    ) mOptions.Debug_Texture = "surface";
            else if ( mOptions.Debug_Texture == "surface"    ) mOptions.Debug_Texture = "normals";
            else if ( mOptions.Debug_Texture == "normals"    ) mOptions.Debug_Texture = "specular";
            else if ( mOptions.Debug_Texture == "specular"   ) mOptions.Debug_Texture = "lighting";
            else if ( mOptions.Debug_Texture == "lighting"   ) mOptions.Debug_Texture = "volumetric";
            else if ( mOptions.Debug_Texture == "volumetric" ) mOptions.Debug_Texture = "ssao";
            else if ( mOptions.Debug_Texture == "ssao"       ) mOptions.Debug_Texture = "bloom";
            else if ( mOptions.Debug_Texture == "bloom"      ) mOptions.Debug_Texture = "";

            mDebugOverlay->notify("debug texture set to \"" + mOptions.Debug_Texture + "\"");

            refresh_options();
        }

        #ifdef SQEE_DEBUG

        if (data.keyboard.key == Key::Num_1)
        {
            sqeeDebugToggle1 = !sqeeDebugToggle1;
            notify(sqeeDebugToggle1, "sqeeDebugToggle1 set to ", {"false", "true"});
            refresh_options();
        }

        if (data.keyboard.key == Key::Num_2)
        {
            sqeeDebugToggle2 = !sqeeDebugToggle2;
            notify(sqeeDebugToggle2, "sqeeDebugToggle2 set to ", {"false", "true"});
            refresh_options();
        }

        #endif
    }
}
