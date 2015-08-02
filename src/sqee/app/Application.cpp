#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/debug/OpenGL.hpp>
#include <sqee/app/Logging.hpp>
#include <sqee/app/Application.hpp>
#include <sqee/app/SettingsMaps.hpp>
#include <sqee/app/PreProcessor.hpp>
#include <sqee/sounds/SoundManager.hpp>
#include <sqee/scripts/Intergration.hpp>

using namespace sq;

Application::~Application() = default;

Application::Application() {
    context.depthBits = 24u,
    context.stencilBits = 8u,
    context.majorVersion = 3u,
    context.minorVersion = 3u,
    context.antialiasingLevel = 0u;
    context.attributeFlags = sf::ContextSettings::Core;
    gl::sys::LoadFunctions();

    sf::Uint32 wStyle = sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize;
    window.create({800u, 600u}, "", wStyle, context);

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

    settings.reset(new SettingsMaps());
    preprocs.reset(new PreProcessor());
    soundman.reset(new SoundManager());

    settings->add<int>("framelimit", 0);
    settings->add<bool>("keyrepeat", false);
    settings->add<bool>("resizable", false);
    settings->add<bool>("fullscreen", false);
    settings->add<bool>("hidecursor", false);
    settings->add<int>("winwidth", 800u);
    settings->add<int>("winheight", 600u);
    settings->add<int>("fullwidth", 800u);
    settings->add<int>("fullheight", 600u);
    settings->add<string>("apptitle", "SQEE Application");

    cs.reset(make_ChaiScript());
    cs_setup_maths(*cs);
    cs_setup_application(*cs);
    cs_setup_settings(*cs);
    cs_setup_console(*cs);
    cs_setup_render(*cs);

    cs->add_global(chai::var(this), "application");
    cs->add_global(chai::var(settings.get()), "settings");

    update();
}

int Application::run() {
    retCode = -1;
    sf::Clock clockFT;

    while (retCode == -1) {
        for (auto& key : sceneSweep) {
            auto ptr = sceneMap.at(key);
            auto func = [ptr](unique_ptr<Scene>& val) { return val.get() == ptr; };
            auto iter = std::find_if(sceneDeq.begin(), sceneDeq.end(), func);
            sceneDeq.erase(iter); sceneMap.erase(key);
        } sceneSweep.clear();

        for (auto& key : handlerSweep) {
            auto ptr = handlerMap.at(key);
            auto func = [ptr](unique_ptr<Handler>& val) { return val.get() == ptr; };
            auto iter = std::find_if(handlerDeq.begin(), handlerDeq.end(), func);
            handlerDeq.erase(iter); handlerMap.erase(key);
        } sceneSweep.clear();

        soundman->clean();

        static sf::Event event;
        while (window.pollEvent(event))
            for (auto& handler : handlerDeq)
                if (handler->handle(event)) break;

        float ft = clockFT.restart().asSeconds();

        for (auto& scene : sceneDeq) {
            scene->accum += ft;
            double dt = 1.0 / double(scene->tickRate);
            while (scene->accum >= dt) {
                scene->update();
                scene->accum -= dt;
            }
        }

        for (auto& scene : sceneDeq) {
            scene->render(ft);
        }

        window.display();
    }

    return retCode;
}

void Application::quit(int _code) {
    retCode = _code;
}

void Application::update() {
    int framelimit = settings->crnt<int>("framelimit");
    bool keyrepeat = settings->crnt<bool>("keyrepeat");
    bool resizable = settings->crnt<bool>("resizable");
    bool fullscreen = settings->crnt<bool>("fullscreen");
    bool hidecursor = settings->crnt<bool>("hidecursor");
    uint winwidth = settings->crnt<int>("winwidth");
    uint winheight = settings->crnt<int>("winheight");
    uint fullwidth = settings->crnt<int>("fullwidth");
    uint fullheight = settings->crnt<int>("fullheight");
    string apptitle = settings->crnt<string>("apptitle");

//    if (fullscreen == true) {
//        sf::Uint32 wStyle = sf::Style::Fullscreen;
//        window.create(){fullwidth, fullheight}, apptitle, wStyle, context);
//    } else {
//        sf::Uint32 wStyle = resizable ? sf::Style::Resize : sf::Style::Titlebar;
//        window.create({winwidth, winheight}, apptitle, wStyle, context);
//    }

    window.setTitle(apptitle);

    window.setVerticalSyncEnabled(framelimit == 2);
    window.setFramerateLimit(framelimit == 1 ? 75u : 0u);
    window.setMouseCursorVisible(!hidecursor);
    window.setKeyRepeatEnabled(keyrepeat);

    if (window.getSize() != sf::Vector2u(get_size().x, get_size().y))
        window.setSize({get_size().x, get_size().y});

    for (auto& scene : sceneDeq) scene->update_settings();
    for (auto& handler : handlerDeq) handler->update_settings();
}

uvec2 Application::get_size() {
    uint width = settings->crnt<int>("winwidth");
    uint height = settings->crnt<int>("winheight");
    return uvec2(width, height);
}

fvec2 Application::mouse_relatify() {
    sf::Vector2i winCentre(window.getSize() / 2u);
    sf::Vector2i mouseMove = winCentre - sf::Mouse::getPosition(window);
    sf::Mouse::setPosition(winCentre, window);
    return fvec2(mouseMove.x, mouseMove.y);
}

void Application::sweep_handler(const string& _id) {
    handlerSweep.emplace(_id);
}

void Application::sweep_scene(const string& _id) {
    sceneSweep.emplace(_id);
}
