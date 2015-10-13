#pragma once
#include <sqee/forward.hpp>
#include <unordered_set>

#include <sqee/app/Settings.hpp>
#include <sqee/app/ChaiConsole.hpp>
#include <sqee/app/DebugOverlay.hpp>
#include <sqee/app/PreProcessor.hpp>
#include <sqee/misc/OrderedMap.hpp>

namespace sf { class Window; class ContextSettings; class Event; }
namespace chaiscript { class ChaiScript; }

namespace sq {

/// The basic SQEE Application class
class Application : NonCopyable {
public:
    Application();
    virtual ~Application();

    virtual int run();
    virtual void quit(int _code);
    virtual void refresh();

    fvec2 mouse_centre();
    uvec2 get_size() const;

    template<class T> T& append_scene(const string& _key);
    template<class T> T& prepend_scene(const string& _key);
    template<class T> T& get_scene(const string& _key);

    template<class T> T& append_handler(const string& _key);
    template<class T> T& prepend_handler(const string& _key);
    template<class T> T& get_handler(const string& _key);

    void sweep_scene(const string& _key);
    void sweep_handler(const string& _key);

    unique_ptr<chai::ChaiScript> cs;

    Settings settings;
    PreProcessor preprocs;
    DebugOverlay overlay;
    ChaiConsole console;

protected:
    unique_ptr<sf::ContextSettings> context;
    unique_ptr<sf::Window> window;
    int retCode = -1;

    OrderedMap<string, unique_ptr<Scene>> sceneMap;
    OrderedMap<string, unique_ptr<Handler>> handlerMap;

    std::unordered_set<string> sceneSweep;
    std::unordered_set<string> handlerSweep;

    virtual bool handle_default(sf::Event _event);
};


template<class T>
T& Application::append_scene(const string& _key) {
    sceneMap.append(_key, new T(this));
    return static_cast<T&>(*sceneMap.back());
}
template<class T>
T& Application::prepend_scene(const string& _key) {
    sceneMap.prepend(_key, new T(this));
    return static_cast<T&>(*sceneMap.front());
}
template<class T>
T& Application::get_scene(const string& _key) {
    return static_cast<T&>(*sceneMap.get(_key));
}

template<class T>
T& Application::append_handler(const string& _key) {
    handlerMap.append(_key, new T(this));
    return static_cast<T&>(*handlerMap.back());
}
template<class T>
T& Application::prepend_handler(const string& _key) {
    handlerMap.prepend(_key, new T(this));
    return static_cast<T&>(*handlerMap.front());
}
template<class T>
T& Application::get_handler(const string& _key) {
    return static_cast<T&>(*handlerMap.get(_key));
}

}
