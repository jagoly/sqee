#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <vector>
#include <forward_list>
#include <memory>

#include <SFML/Graphics.hpp>

#include <libsqee/scenes/scene.hpp>
#include <libsqee/logic/entity.hpp>


namespace sq {

class Handler;

class Application {
public:
    Application();

    bool running;
    sf::Vector2u windowSize;

    sf::RenderWindow* window;
    void run();
    void attach_handler(Handler*);
    void append_scene(Scene*);
    void add_entity(std::string, Entity*);
    void set_size(sf::Vector2u);

    std::forward_list<std::unique_ptr<Handler>> handlerFList;
    std::map<std::string, std::unique_ptr<Entity>> entityMap;
    std::vector<std::unique_ptr<Scene>> sceneVector;
};

}

#endif // APPLICATION_HPP
