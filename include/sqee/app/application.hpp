#pragma once
#include <sqee/forward.hpp>

#include <set>

#include <SFML/Window/Window.hpp>

#include <sqee/app/settings.hpp>
#include <sqee/events/handler.hpp>
#include <sqee/gl/preprocessor.hpp>
#include <sqee/misc/indexedmap.hpp>
#include <sqee/scenes/scene.hpp>
#include <sqee/scripts/chaiscript.hpp>
#include <sqee/sounds/soundmanager.hpp>

namespace sq {

class Application : NonCopyable {
public:
    Application(uvec2 _size, bool _resizable);

    int run();
    void quit(int _code);

    void set_size(uvec2 _size);
    uvec2 get_size();

    vec2 mouse_relatify();

    void sweep_handler(const string& _id);
    void sweep_scene(const string& _id);

    IndexedMap<string, unique_ptr<Scene>> sceneIM;
    IndexedMap<string, unique_ptr<Handler>> handlerIM;

    chai::ChaiScript cs;
    SoundManager sndMan;
    SettingMap settings;
    Preprocessor preproc;

protected:
    sf::Window window;
    int retCode;

    std::set<string> handlerSweep;
    std::set<string> sceneSweep;

    void update_settings();
};

}
