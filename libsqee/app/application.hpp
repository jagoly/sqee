#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <vector>
#include <forward_list>
#include <memory>

#include <SFML/Graphics.hpp>

#include <libsqee/visuals/scene.hpp>
#include <libsqee/visuals/textureholder.hpp>

namespace sq {

class Handler;
class Stage;

class Application {
public:
    Application();

    bool running;
    int tickRate;
    sf::Vector2u windowSize;

    sf::RenderWindow* window;
    TextureHolder textureHolder;
    sf::Clock clock;

    virtual void run();
    void attach_handler(Handler*);
    void append_scene(Scene*);
    void add_stage(std::string, Stage*);
    void set_size(sf::Vector2u);

    std::forward_list<std::unique_ptr<Handler>> handlerFList;
    std::map<std::string, std::unique_ptr<Stage>> stageMap;
    std::vector<std::unique_ptr<Scene>> sceneVector;
};

}

#endif // APPLICATION_HPP
