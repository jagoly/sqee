#pragma once

#include <unordered_set>

#include <sqee/builtins.hpp>
#include <sqee/app/Settings.hpp>
#include <sqee/app/MessageBus.hpp>
#include <sqee/app/ChaiConsole.hpp>
#include <sqee/app/DebugOverlay.hpp>
#include <sqee/app/PreProcessor.hpp>
#include <sqee/misc/OrderedMap.hpp>
#include <sqee/maths/Vectors.hpp>


// Forward Declarations /////
namespace sq { class Scene; }
namespace sf { class Window; class Event; }
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

    Vec2F mouse_centre();
    Vec2U get_size() const;

    template<class T> T& append_scene(const string& _key);
    template<class T> T& prepend_scene(const string& _key);
    template<class T> T& get_scene(const string& _key);

    void sweep_scene(const string& _key);

    unique_ptr<chai::ChaiScript> cs;

    MessageBus messageBus;

    Settings settings;
    PreProcessor preprocs;
    DebugOverlay overlay;
    ChaiConsole console;

protected:
    unique_ptr<sf::Window> window;
    int retCode = -1;

    OrderedMap<string, unique_ptr<Scene>> sceneMap;

    std::unordered_set<string> sceneSweep;

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

}
