#pragma once
#include <sqee/forward.hpp>

#include <SFML/Window/Window.hpp>

#include <sqee/misc/indexedmap.hpp>
#include <sqee/scripts/chaiscript.hpp>
#include <sqee/handlers/handler.hpp>
#include <sqee/scenes/scene.hpp>

namespace sq {

class Application : NonCopyable {
public:
    Application(bool _resizable);
    virtual ~Application();

    int run();
    void quit(int _code);

    void update();
    uvec2 get_size();
    vec2 mouse_relatify();

    void sweep_handler(const string& _id);
    void sweep_scene(const string& _id);

    IndexedMap<string, unique_ptr<Scene>> sceneIM;
    IndexedMap<string, unique_ptr<Handler>> handlerIM;

    template <class T>
    T* get_scene(const string& _key);
    template<class T>
    T* get_handler(const string& _key);

    unique_ptr<SettingsMaps> settings;
    unique_ptr<PreProcessor> preprocs;
    unique_ptr<SoundManager> soundman;
    unique_ptr<chai::ChaiScript> cs;

protected:
    sf::Window window;
    int retCode;

    unordered_set<string> handlerSweep;
    unordered_set<string> sceneSweep;
};

template<class T>
T* Application::get_scene(const string& _key) {
    return static_cast<T*>(sceneIM.get(_key).get());
}

template<class T>
T* Application::get_handler(const string& _key) {
    return static_cast<T*>(handlerIM.get(_key).get());
}

}
