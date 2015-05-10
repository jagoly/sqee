#pragma once
#include <sqee/forward.hpp>

#include <SFML/Window/Window.hpp>

#include <sqee/app/settings.hpp>
#include <sqee/gl/preprocessor.hpp>
#include <sqee/sounds/soundmanager.hpp>
#include <sqee/misc/indexedmap.hpp>
#include <sqee/scripts/chaiscript.hpp>
#include <sqee/events/handler.hpp>
#include <sqee/scenes/scene.hpp>

namespace sq {

class Application : NonCopyable {
public:
    Application(bool _resizable, uvec2 _size);

    int run();
    void quit(int _code);

    void resize_scenes(uvec2 _size);
    void set_size(uvec2 _size);
    uvec2 get_size();

    vec2 mouse_relatify();

    void sweep_handler(const string& _id);
    void sweep_scene(const string& _id);

    IndexedMap<string, unique_ptr<Scene>> sceneIM;
    IndexedMap<string, unique_ptr<Handler>> handlerIM;

    SettingMap settings;
    Preprocessor preproc;
    SoundManager soundMan;

    unique_ptr<chai::ChaiScript> cs;

protected:
    sf::Window window;
    int retCode;

    unordered_set<string> handlerSweep;
    unordered_set<string> sceneSweep;

    void update_settings();
};

}
