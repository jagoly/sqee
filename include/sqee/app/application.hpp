#pragma once

#include <gl/gl.hpp>
#include <scenes/scene.hpp>
#include <events/handler.hpp>
#include <sounds/soundmanager.hpp>
#include <misc/containers.hpp>

namespace sq {

class Application {
public:
    Application(glm::uvec2 _size, bool _resizable, const string& _title);

    SoundManager soundManager;

    int run();
    void quit(int _code);

    void set_size(glm::uvec2 _size);
    glm::uvec2 get_size();

    bool vsync = true;
    void set_vsync(bool _vsync);

    void sweep_handler(const string& _id);
    void sweep_scene(const string& _id);

    IndexedMap<string, Scene::Ptr> sceneIM;
    IndexedMap<string, Handler::Ptr> handlerIM;

protected:
    sf::Window window;
    int retCode;

    set<string> handlerSweep;
    set<string> sceneSweep;

    string title;
};

}
