#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>

#include <libsqee/scenes/scene.hpp>


namespace sq {

class EventHandler;

class Application {
public:
    Application();

    bool running;
    sf::Vector2u windowSize;

    sf::RenderWindow* window;
    void run();
    void attach_handler(EventHandler*);
    void append_scene(Scene*);
    void set_size(sf::Vector2u);
private:
    std::vector<std::unique_ptr<EventHandler>> handlerVector;
    std::vector<std::unique_ptr<Scene>> sceneVector;
};

}

#endif // APPLICATION_HPP
