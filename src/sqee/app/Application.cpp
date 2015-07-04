#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/debug/Callbacks.hpp>
#include <sqee/gl/Preprocessor.hpp>
#include <sqee/app/Application.hpp>
#include <sqee/app/SettingsMap.hpp>
#include <sqee/app/Logging.hpp>
#include <sqee/sounds/SoundManager.hpp>
#include <sqee/scripts/Intergration.hpp>

using namespace sq;

Application::~Application() = default;

Application::Application(bool _resizable) {
    sf::ContextSettings sfmlSettings;
    sfmlSettings.depthBits = 24u,
    sfmlSettings.stencilBits = 8u,
    sfmlSettings.majorVersion = 3u,
    sfmlSettings.minorVersion = 3u,
    sfmlSettings.antialiasingLevel = 0u;
    sfmlSettings.attributeFlags = sf::ContextSettings::Core;
    sf::Uint32 winStyle = sf::Style::Close | sf::Style::Titlebar;
    if (_resizable) winStyle = winStyle | sf::Style::Resize;
    window.create({800, 600}, "", winStyle, sfmlSettings);

    settings.reset(new SettingsMaps());
    preprocs.reset(new PreProcessor());
    soundman.reset(new SoundManager());

    gl::sys::LoadFunctions();

    #ifdef SQEE_DEBUG
    const GLubyte* renderer = gl::GetString(gl::RENDERER);
    const GLubyte* version = gl::GetString(gl::VERSION);
    log_info("Renderer %s", (char*)renderer);
    log_info("Version %s", (char*)version);
    gl::Enable(gl::DEBUG_OUTPUT);
    gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS);
    gl::DebugMessageControl(gl::DEBUG_SOURCE_API, gl::DEBUG_TYPE_OTHER,
                            gl::DEBUG_SEVERITY_NOTIFICATION, 0, 0, false);
    gl::DebugMessageCallback(debug_callback, nullptr);
    #endif

    settings->add<int>("framelimit", 0);
    settings->add<bool>("keyrepeat", false);
    settings->add<string>("app_title", "SQEE Application");
    settings->add<int>("app_width", 800u);
    settings->add<int>("app_height", 600u);

    cs.reset(make_ChaiScript());
    cs_setup_maths(*cs);
    cs_setup_application(*cs);
    cs_setup_settings(*cs);
    cs_setup_console(*cs);
    cs_setup_render(*cs);

    cs->add_global(chai::var(this), "application");
    cs->add_global(chai::var(settings.get()), "settings");
}

int Application::run() {
    update();
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
    int limitmode = settings->crnt<int>("framelimit");
    window.setTitle(settings->crnt<string>("app_title"));
    window.setKeyRepeatEnabled(settings->crnt<bool>("keyrepeat"));
    window.setFramerateLimit(limitmode == 1 ? 75u : 0u);
    window.setVerticalSyncEnabled(limitmode == 2);

    if (window.getSize() != sf::Vector2u(get_size().x, get_size().y))
        window.setSize({get_size().x, get_size().y});

    for (auto& scene : sceneDeq) scene->update_settings();
    for (auto& handler : handlerDeq) handler->update_settings();
}

uvec2 Application::get_size() {
    uint width = settings->crnt<int>("app_width");
    uint height = settings->crnt<int>("app_height");
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
