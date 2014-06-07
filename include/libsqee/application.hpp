#pragma once

#include <list>
#include <forward_list>
#include <set>
#include <memory>

#include <SFML/Graphics.hpp>
#include <SFGUI/SFGUI.hpp>

#include <scene.hpp>
#include <textureholder.hpp>
#include <handler.hpp>

namespace sq {

class Application {
public:
    Application();

    bool running;
    bool freeze;
    bool vsync;

    sf::RenderWindow* window;
    TextureHolder texHolder;
    sf::Clock clock;

    sfg::SFGUI sfgui;

    virtual void run();
    virtual void set_size(sf::Vector2u);

    void attach_handler(std::string, std::shared_ptr<Handler>);

    void append_scene(std::string, std::shared_ptr<Scene>);
    void prepend_scene(std::string, std::shared_ptr<Scene>);
    void insert_scene(int, std::string, std::shared_ptr<Scene>);

    Scene& get_scene_first();
    Scene& get_scene_last();
    //Scene& get_scene(int);
    Scene& get_scene(std::string);

    void sweep_handler(std::string);
    void sweep_scene(std::string);

protected:
    std::map<std::string, std::shared_ptr<Handler>> handlerMap;
    std::forward_list<std::shared_ptr<Handler>> handlerFList;
    std::map<std::string, std::shared_ptr<Scene>> sceneMap;
    std::list<std::shared_ptr<Scene>> sceneList;

    std::set<std::string> handlerSweep;
    std::set<std::string> sceneSweep;
};

}
