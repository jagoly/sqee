#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <vector>

#include <SFML/Graphics.hpp>

namespace sq {

class EventHandler;
class Scene;

class Application {
public:
    Application();

    bool running;

    sf::RenderWindow* window;
    void run();
    void attach_handler(EventHandler);
private:
    std::vector<EventHandler>* handlerVector;
    Scene* scene;
};

}

#endif // APPLICATION_HPP
