#pragma once

#include <list>
#include <forward_list>
#include <set>
#include <memory>

#include <SFML/Graphics.hpp>
#include <SFGUI/SFGUI.hpp>

#include <scene.hpp>
#include <textureholder.hpp>
#include <fontholder.hpp>
#include <soundmanager.hpp>
#include <handler.hpp>

namespace sq {

class Application {
public:
    Application(uint, uint, uint, bool, bool, bool, std::string);

    bool running;

    TextureHolder texHolder;
    FontHolder fontHolder;
    SoundManager soundManager;

    sfg::SFGUI sfgui;

    void run();

    virtual void set_size(uint width, uint height);
    sf::Vector2u get_size();

    void set_vsync(bool);
    bool get_vsync();

    void set_showfps(bool);
    bool get_showfps();

    void attach_handler(std::string, HandlerPtr);

    void append_scene(std::string, ScenePtr);
    void prepend_scene(std::string, ScenePtr);
    void insert_scene(int, std::string, ScenePtr);

    Scene& get_scene_first();
    Scene& get_scene_last();
    Scene& get_scene(std::string);

    void sweep_handler(std::string);
    void sweep_scene(std::string);

protected:
    sf::RenderWindow* window;

    std::map<std::string, HandlerPtr> handlerMap;
    std::forward_list<HandlerPtr> handlerFList;
    std::map<std::string, ScenePtr> sceneMap;
    std::list<ScenePtr> sceneList;

    std::set<std::string> handlerSweep;
    std::set<std::string> sceneSweep;

    bool vsync;
    bool showfps;
};

}
