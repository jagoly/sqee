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
    sf::Vector2u windowSize;

    sf::RenderWindow* window;
    TextureHolder textureHolder;
    sf::Clock clock;

    virtual void run();
    virtual void set_size(sf::Vector2u);

    void attach_handler(std::unique_ptr<Handler>);
    void append_scene(std::unique_ptr<Scene>);
    void add_stage(std::string, std::unique_ptr<Stage>);

    Stage& get_stage(std::string);
    Scene& get_scene(int);
    Scene& get_scene();

protected:
    std::forward_list<std::unique_ptr<Handler>> handlerFList;
    std::map<std::string, std::unique_ptr<Stage>> stageMap;
    std::vector<std::unique_ptr<Scene>> sceneVector;

};

}

#endif // APPLICATION_HPP
