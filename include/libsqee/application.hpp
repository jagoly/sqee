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

    void attach_handler(std::unique_ptr<Handler>);

    void append_scene(std::string, std::shared_ptr<Scene>);
    void prepend_scene(std::string, std::shared_ptr<Scene>);
    void insert_scene(int, std::string, std::shared_ptr<Scene>);

    Scene& get_scene();
    Scene& get_scene(int);
    Scene& get_scene(std::string);


protected:
    std::forward_list<std::unique_ptr<Handler>> handlerFList;
    std::map<std::string, std::shared_ptr<Scene>> sceneMap;
    std::list<std::shared_ptr<Scene>> sceneList;
};

}
