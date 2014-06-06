#pragma once

#include <list>
#include <forward_list>
#include <memory>

#include <SFML/Graphics.hpp>

#include <scene.hpp>
#include <textureholder.hpp>
#include <handler.hpp>

namespace sq {

class Handler;

class Application {
public:
    Application();

    bool running;
    bool freeze;
    bool vsync;

    sf::RenderWindow* window;
    TextureHolder texHolder;
    sf::Clock clock;

    virtual void run();
    virtual void set_size(sf::Vector2u);

    void attach_handler(std::string, std::shared_ptr<Handler>);
    void remove_handler(std::string);

    void append_scene(std::string, std::shared_ptr<Scene>);
    void prepend_scene(std::string, std::shared_ptr<Scene>);
    void insert_scene(int, std::string, std::shared_ptr<Scene>);

    Scene& get_scene_first();
    Scene& get_scene_last();
    //Scene& get_scene(int);
    Scene& get_scene(std::string);

    void pop_scene(std::string);


protected:
    std::map<std::string, std::shared_ptr<Handler>> handlerMap;
    std::forward_list<std::shared_ptr<Handler>> handlerFList;
    std::map<std::string, std::shared_ptr<Scene>> sceneMap;
    std::list<std::shared_ptr<Scene>> sceneList;
};

}
