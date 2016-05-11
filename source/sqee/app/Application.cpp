#include <SFML/Window/Window.hpp>
#include <SFML/Window/Event.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/debug/OpenGL.hpp>
#include <sqee/sounds/SoundManager.hpp>
#include <sqee/scripts/ChaiScript.hpp>
#include <sqee/scripts/BasicSetup.hpp>
#include <sqee/scenes/Scene.hpp>
#include <sqee/app/Application.hpp>

using namespace sq;


Application::~Application() = default;

Application::Application() : overlay(this), console(this) {
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
    gl::DebugMessageControl(gl::DEBUG_SOURCE_API, gl::DEBUG_TYPE_OTHER,
                            gl::DEBUG_SEVERITY_NOTIFICATION, 0, 0, false);
    gl::DebugMessageCallback(debug_callback, nullptr);
    #endif

    cs.reset(make_ChaiScript());

    cs_setup_app(*cs);
    cs_setup_render(*cs);
    cs_setup_maths(*cs);
    cs_setup_ecs(*cs);

    settings.add<int>("app_fpslimit", 0);
    settings.add<bool>("app_keyrepeat", false);
    settings.add<bool>("app_resizable", false);
    settings.add<bool>("app_fullscreen", false);
    settings.add<bool>("app_hidecursor", false);
    settings.add<int>("app_width", 800u);
    settings.add<int>("app_height", 600u);
    settings.add<string>("app_title", "SQEE Application");

    cs->add_global(chai::var(this), "app");

    refresh();
}

int Application::run() {
    sf::Clock clockFT;

    while (retCode == -1) {
        for (auto& key : sceneSweep)
            sceneMap.remove(key);
        sceneSweep.clear();

        //soundman->clean();

        sf::Event event;
        while (window->pollEvent(event))
            if (handle_default(event) == false)
                for (auto& scene : sceneMap)
                    if (scene->handle(event)) break;

        float ft = clockFT.restart().asSeconds();

        for (auto& scene : sceneMap) {
            scene->accum += ft;
            double dt = 1.0 / double(scene->tickRate);
            while (scene->accum >= dt) {
                scene->accum -= dt;
                scene->update();
            }
        }

        for (auto& scene : sceneMap) {
            scene->render();
        }

        // Console
        if (console.active) {
            console.accum += ft;
            while (console.accum >= 0.5) {
                console.accum -= 0.5;
                console.update();
            } console.render();
        }

        // Overlay
        if (overlay.active) {
            overlay.accum += ft;
            while (overlay.accum >= 0.125) {
                overlay.accum -= 0.125;
                overlay.update();
            } overlay.render(ft);
        }

        window->display();
    }

    return retCode;
}

void Application::quit(int _code) {
    retCode = _code;
}

void Application::refresh() {
    int fpslimit = settings.get<int>("app_fpslimit");
    bool resizable = settings.get<bool>("app_resizable");
    bool keyrepeat = settings.get<bool>("app_keyrepeat");
    bool fullscreen = settings.get<bool>("app_fullscreen");
    bool hidecursor = settings.get<bool>("app_hidecursor");
    uint width = settings.get<int>("app_width");
    uint height = settings.get<int>("app_height");
    string title = settings.get<string>("app_title");

//    if (fullscreen == true) {
//        sf::Uint32 wStyle = sf::Style::Fullscreen;
//        window.create(){fullwidth, fullheight}, apptitle, wStyle, context);
//    } else {
//        sf::Uint32 wStyle = resizable ? sf::Style::Resize : sf::Style::Titlebar;
//        window.create({winwidth, winheight}, apptitle, wStyle, context);
//    }

    window->setTitle(title);

    window->setVerticalSyncEnabled(fpslimit == 2);
    window->setFramerateLimit(fpslimit == 1 ? 75u : 0u);
    window->setMouseCursorVisible(!hidecursor);
    window->setKeyRepeatEnabled(keyrepeat);

    if (window->getSize() != sf::Vector2u(get_size().x, get_size().y))
        window->setSize({get_size().x, get_size().y});

    for (auto& scene : sceneMap) scene->refresh();
}


bool Application::handle_default(sf::Event _event) {
    if (_event.type == sf::Event::Closed) {
        quit(0); return true;
    }

    if (_event.type == sf::Event::Resized) {
        settings.mod<int>("app_width", _event.size.width);
        settings.mod<int>("app_height", _event.size.height);
        refresh(); return true;
    }

    if (_event.type == sf::Event::KeyPressed) {
        if (_event.key.code == sf::Keyboard::Menu) {
            if (_event.key.control) console.toggle_active();
            else overlay.toggle_active(); return true;
        }
    }

    if (console.active == true && (
          _event.type == sf::Event::KeyPressed ||
          _event.type == sf::Event::KeyReleased ||
          _event.type == sf::Event::TextEntered ||
          _event.type == sf::Event::MouseButtonPressed ||
          _event.type == sf::Event::MouseButtonReleased ||
          _event.type == sf::Event::MouseWheelScrolled )) {
        console.handle_input(_event); return true;
    }

    return false;
}


Vec2F Application::mouse_centre() {
    sf::Vector2i winCentre(window->getSize() / 2u);
    sf::Vector2i mouseMove = winCentre - sf::Mouse::getPosition(*window);
    sf::Mouse::setPosition(winCentre, *window);
    return Vec2F(mouseMove.x, mouseMove.y);
}

Vec2U Application::get_size() const {
    uint width = settings.get<int>("app_width");
    uint height = settings.get<int>("app_height");
    return Vec2U(width, height);
}

void Application::sweep_scene(const string& _id) {
    sceneSweep.emplace(_id);
}
