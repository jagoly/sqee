#include <SFML/Window/Window.hpp>
#include <SFML/Window/Event.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/debug/OpenGL.hpp>
#include <sqee/scripts/ChaiScript.hpp>
#include <sqee/scripts/BasicSetup.hpp>
#include <sqee/scenes/Scene.hpp>
#include <sqee/app/DebugOverlay.hpp>
#include <sqee/app/ChaiConsole.hpp>
#include <sqee/app/Application.hpp>

using namespace sq;

Application::~Application() = default;

Application::Application() {
    chaiEngine = create_ChaiEngine();
    overlay.reset(new DebugOverlay(*this));
    console.reset(new ChaiConsole(*this));

    sf::Uint32 wStyle = sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize;
    sf::ContextSettings context(24u, 8u, 0u, 4u, 2u, sf::ContextSettings::Core);
    window.reset(new sf::Window({800u, 600u}, "", wStyle, context));

    gl::sys::LoadFunctions();

    #ifdef SQEE_DEBUG
    const GLubyte* renderer = gl::GetString(gl::RENDERER);
    const GLubyte* version = gl::GetString(gl::VERSION);
    log_info("Renderer: %s", (char*)renderer);
    log_info("Version: %s", (char*)version);
    gl::Enable(gl::DEBUG_OUTPUT);
    gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS);
    gl::DebugMessageControl(gl::DEBUG_SOURCE_API, gl::DEBUG_TYPE_OTHER, gl::DEBUG_SEVERITY_NOTIFICATION, 0, 0, false);
    gl::DebugMessageCallback(debug_callback, nullptr);
    #endif

    chaiscript_setup_app(*chaiEngine);
    chaiscript_setup_entity(*chaiEngine);
    chaiscript_setup_physics(*chaiEngine);
    chaiscript_setup_maths(*chaiEngine);
    chaiscript_setup_messages(*chaiEngine);

    chaiEngine->add_global(chai::var(this), "app");
}

int Application::run() {
    update_options();

    sf::Clock clock;
    sf::Event event;

    while (returnCode == -1) {

        // poll and handle events
        while (window->pollEvent(event))
            if (this->handle(event) == false)
                for (auto& scene : activeScenes)
                    if (scene->handle(event) == true)
                        break;

        float frameTime = clock.restart().asSeconds();

        // tick scenes
        for (auto& scene : activeScenes) {
            scene->accumulation += frameTime;
            while (scene->accumulation >= scene->tickTime) {
                scene->accumulation -= scene->tickTime;
                scene->tick(); }
        }

        // render scenes
        for (auto& scene : activeScenes)
            scene->render();

        // tick and render console
        if (console->active == true) {
            console->accumulation += frameTime;
            while (console->accumulation >= 0.5) {
                console->accumulation -= 0.5;
                console->tick(); }
            console->render();
        }

        // tick and render overlay
        if (overlay->active == true) {
            overlay->accumulation += frameTime;
            while (overlay->accumulation >= 0.125) {
                overlay->accumulation -= 0.125;
                overlay->tick(); }
            overlay->render(frameTime);
        }

        window->display();
    }

    return returnCode;
}

void Application::quit(int _code) {
    returnCode = _code;
}

void Application::update_options() {
    if (window->getSize() != sf::Vector2u(OPTION_WindowSize.x, OPTION_WindowSize.y))
        window->setSize(sf::Vector2u(OPTION_WindowSize.x, OPTION_WindowSize.y));

    window->setVerticalSyncEnabled(OPTION_VerticalSync);
    window->setMouseCursorVisible(!OPTION_HideCursor);
    window->setKeyRepeatEnabled(OPTION_KeyRepeat);
    window->setTitle(OPTION_WindowTitle);

    for (auto& scene : activeScenes) scene->update_options();
}

bool Application::handle(sf::Event _event) {
    if (_event.type == sf::Event::Closed) {
        quit(0); return true;
    }

    if (_event.type == sf::Event::Resized) {
        OPTION_WindowSize.x = _event.size.width;
        OPTION_WindowSize.y = _event.size.height;
        update_options(); return true;
    }

    if (_event.type == sf::Event::KeyPressed) {
        if (_event.key.code == sf::Keyboard::Menu) {
            if (_event.key.control) console->toggle_active();
            else overlay->toggle_active(); return true;
        }
    }

    if (console->active == true && (
          _event.type == sf::Event::KeyPressed ||
          _event.type == sf::Event::KeyReleased ||
          _event.type == sf::Event::TextEntered ||
          _event.type == sf::Event::MouseButtonPressed ||
          _event.type == sf::Event::MouseButtonReleased ||
          _event.type == sf::Event::MouseWheelScrolled )) {
        console->handle_input(_event); return true;
    }

    return false;
}

Vec2F Application::mouse_centre() {
    sf::Vector2i winCentre(window->getSize() / 2u);
    sf::Vector2i mouseMove = winCentre - sf::Mouse::getPosition(*window);
    sf::Mouse::setPosition(winCentre, *window);
    return Vec2F(mouseMove.x, mouseMove.y);
}
