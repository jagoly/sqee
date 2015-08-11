#pragma once
#include <sqee/forward.hpp>

#include <SFML/Window/Window.hpp>

#include <sqee/scripts/ChaiScript.hpp>
#include <sqee/handlers/Handler.hpp>
#include <sqee/scenes/Scene.hpp>

namespace sq {

class Application : NonCopyable {
public:
    Application();
    virtual ~Application();

    int run();
    void quit(int _code);

    void update();
    uvec2 get_size();
    fvec2 mouse_relatify();

    template <class T>
    T* append_scene(const string& _key);
    template <class T>
    T* prepend_scene(const string& _key);
    template <class T>
    T* get_scene(const string& _key);

    template <class T>
    T* append_handler(const string& _key);
    template <class T>
    T* prepend_handler(const string& _key);
    template<class T>
    T* get_handler(const string& _key);

    void sweep_scene(const string& _key);
    void sweep_handler(const string& _key);

    unique_ptr<Settings> settings;
    unique_ptr<PreProcessor> preprocs;
    unique_ptr<SoundManager> soundman;
    unique_ptr<chai::ChaiScript> cs;

protected:
    sf::ContextSettings context;
    sf::Window window;
    int retCode;

    deque<unique_ptr<Scene>> sceneDeq;
    deque<unique_ptr<Handler>> handlerDeq;
    unordered_map<string, Scene*> sceneMap;
    unordered_map<string, Handler*> handlerMap;

    unordered_set<string> sceneSweep;
    unordered_set<string> handlerSweep;
};

template<class T>
T* Application::append_scene(const string& _key) {
    if (sceneMap.count(_key) != 0u)
        throw string("Scene \""+_key+"\" already in map");
    sceneDeq.emplace_back(new T(this));
    sceneMap.emplace(_key, sceneDeq.back().get());
    return static_cast<T*>(sceneDeq.back().get());
}
template<class T>
T* Application::prepend_scene(const string& _key) {
    sceneDeq.emplace_front(new T(this));
    sceneMap.emplace(_key, sceneDeq.front().get());
    return static_cast<T*>(sceneDeq.front().get());
}
template<class T>
T* Application::get_scene(const string& _key) {
    return static_cast<T*>(sceneMap.at(_key));
}

template<class T>
T* Application::append_handler(const string& _key) {
    handlerDeq.emplace_back(new T(this));
    handlerMap.emplace(_key, handlerDeq.back().get());
    return static_cast<T*>(handlerDeq.back().get());
}
template<class T>
T* Application::prepend_handler(const string& _key) {
    handlerDeq.emplace_front(new T(this));
    handlerMap.emplace(_key, handlerDeq.front().get());
    return static_cast<T*>(handlerDeq.front().get());
}
template<class T>
T* Application::get_handler(const string& _key) {
    return static_cast<T*>(handlerMap.at(_key));
}

}
