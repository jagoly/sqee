#pragma once

#include <set>
#include <memory>

#include <gl/gl.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <scenes/scene.hpp>
#include <gl/textures.hpp>
#include <text/font.hpp>
#include <sounds/soundmanager.hpp>
#include <events/handler.hpp>

namespace sq {

class Application {
public:
    Application(glm::uvec2 _size, bool _showFPS, bool _vSync,
                bool _resizable, const std::string& _title);

    bool running;

    FT_Library ftLib;
    SoundManager soundManager;

    void run();

    void set_size(glm::uvec2);
    glm::uvec2 get_size();

    void set_vSync(bool _vSync);
    bool get_vSync();

    void set_showFPS(bool _showFPS);
    bool get_showFPS();

    void sweep_handler(const std::string& _id);
    void sweep_scene(const std::string& _id);

    IndexedMap<std::string, Scene::Ptr> sceneIM;
    IndexedMap<std::string, Handler::Ptr> handlerIM;
protected:
    sf::Window window;

    std::set<std::string> handlerSweep;
    std::set<std::string> sceneSweep;

    bool vSync;
    bool showFPS;
    std::string title;
};

}
