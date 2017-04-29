#include <SFML/Window/Window.hpp>
#include <SFML/Window/Event.hpp>

#include <sqee/debug/Logging.hpp>
#include <sqee/debug/OpenGL.hpp>

#include <sqee/messages.hpp>
#include <sqee/gl/Context.hpp>

#include <sqee/scripts/ChaiEngine.hpp>
#include <sqee/scripts/BasicSetup.hpp>

#include <sqee/app/ChaiConsole.hpp>
#include <sqee/app/DebugOverlay.hpp>
#include <sqee/app/MessageBus.hpp>

#include <sqee/app/Application.hpp>

using namespace sq;

//============================================================================//

Application::~Application() = default;

Application::Application()
{
    sf::Uint32 wStyle = sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize;
    sf::ContextSettings context(24u, 8u, 0u, 4u, 2u, sf::ContextSettings::Core);
    mWindow = std::make_unique<sf::Window>(sf::VideoMode(800, 600), "", wStyle, context);

    gl::sys::LoadFunctions();

    //========================================================//

    #ifdef SQEE_DEBUG

    const GLubyte* renderer = gl::GetString(gl::RENDERER);
    const GLubyte* version = gl::GetString(gl::VERSION);
    log_info("Renderer: %s", reinterpret_cast<const char*>(renderer));
    log_info("Version: %s", reinterpret_cast<const char*>(version));

    gl::Enable(gl::DEBUG_OUTPUT);
    gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS);

    gl::DebugMessageControl(gl::DEBUG_SOURCE_API, gl::DEBUG_TYPE_OTHER,
                            gl::DEBUG_SEVERITY_NOTIFICATION, 0, 0, false);
    gl::DebugMessageControl(gl::DEBUG_SOURCE_SHADER_COMPILER, gl::DEBUG_TYPE_OTHER,
                            gl::DEBUG_SEVERITY_NOTIFICATION, 0, 0, false);

    gl::DebugMessageCallback(debug_callback, nullptr);

    #endif

    //========================================================//

    mChaiEngine = create_ChaiEngine();

    mChaiConsole = std::make_unique<ChaiConsole>(*this);
    mDebugOverlay = std::make_unique<DebugOverlay>(*this);
    mMessageBus = std::make_unique<MessageBus>();

    //========================================================//

    mMessageBus->register_type<msg::Toggle_Chai_Console>();
    mMessageBus->register_type<msg::Toggle_Debug_Overlay>();

    chaiscript_setup_app(*mChaiEngine);
    chaiscript_setup_physics(*mChaiEngine);
    chaiscript_setup_maths(*mChaiEngine);
    chaiscript_setup_messages(*mChaiEngine);

    mChaiEngine->add_global(chai::var(this), "app");
}

//============================================================================//

int Application::run()
{
    update_options();

    sf::Clock clock;
    sf::Event event;

    while (mReturnCode == -1)
    {
        //========================================================//

        // poll and handle events /////

        while (mWindow->pollEvent(event))
        {
            if (this->handle(event)) continue;

            for (auto& scene : mScenes)
                if (scene->handle(event)) break;
        }

        //========================================================//

        // time since rendering previous frame
        const float frameTime = clock.restart().asSeconds();

        //========================================================//

        // tick and render scenes /////

        for (auto& scene : mScenes)
        {
            scene->accumulation += double(frameTime);

            while (scene->accumulation >= scene->tickTime)
            {
                scene->accumulation -= scene->tickTime;
                scene->tick();
            }
        }

        for (auto& scene : mScenes)
            scene->render();

        //========================================================//

        // tick and render chai console /////

        if (mChaiConsole->is_active() == true)
        {
            mChaiConsole->accumulation += frameTime;

            while (mChaiConsole->accumulation >= 0.5f)
            {
                mChaiConsole->accumulation -= 0.5f;
                mChaiConsole->tick();
            }

            mChaiConsole->render();
        }

        //========================================================//

        // tick and render debug overlay /////

        if (mDebugOverlay->is_active() == true)
        {
            mDebugOverlay->accumulation += frameTime;

            while (mDebugOverlay->accumulation >= 0.125f)
            {
                mDebugOverlay->accumulation -= 0.125f;
                mDebugOverlay->tick();
            }

            mDebugOverlay->render(frameTime);
        }

        //========================================================//

        mWindow->display();
    }

    return mReturnCode;
}

//============================================================================//

void Application::quit(int code)
{
    mReturnCode = code;
}

//============================================================================//

void Application::update_options()
{
    sf::Vector2u windowSize { OPTION_WindowSize.x, OPTION_WindowSize.y };
    if (mWindow->getSize() != windowSize) mWindow->setSize(windowSize);

    mWindow->setVerticalSyncEnabled(OPTION_VerticalSync);
    mWindow->setMouseCursorVisible(!OPTION_HideCursor);
    mWindow->setKeyRepeatEnabled(OPTION_KeyRepeat);
    mWindow->setTitle(OPTION_WindowTitle);

    for (auto& scene : mScenes) scene->update_options();
}

//============================================================================//

bool Application::handle(sf::Event event)
{
    //========================================================//

    if (event.type == sf::Event::Closed)
    {
        quit(0); return true;
    }

    //========================================================//

    if (event.type == sf::Event::Resized)
    {
        OPTION_WindowSize.x = event.size.width;
        OPTION_WindowSize.y = event.size.height;
        update_options(); return true;
    }

    //========================================================//

    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Menu)
        {
            if (event.key.control) mChaiConsole->toggle_active();
            else mDebugOverlay->toggle_active();

            return true;
        }
    }

    //========================================================//

    if (mChaiConsole->is_active() == true)
    {
        if ( event.type == sf::Event::KeyPressed
          || event.type == sf::Event::KeyReleased
          || event.type == sf::Event::TextEntered
          || event.type == sf::Event::MouseButtonPressed
          || event.type == sf::Event::MouseButtonReleased
          || event.type == sf::Event::MouseWheelScrolled
           ) mChaiConsole->handle(event);

        return true; // all events
    }

    //========================================================//

    return false;
}

//============================================================================//

Vec2F Application::mouse_centre()
{
    sf::Vector2i windowCentre(mWindow->getSize() / 2u);
    sf::Vector2i mousePos = sf::Mouse::getPosition(*mWindow);
    sf::Vector2i mouseMove = windowCentre - mousePos;

    sf::Mouse::setPosition(windowCentre, *mWindow);
    return Vec2F(mouseMove.x, mouseMove.y);
}
