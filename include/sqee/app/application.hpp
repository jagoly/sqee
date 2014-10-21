#pragma once

#include <set>
#include <memory>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <gl/gl.hpp>
#include <gl/textures.hpp>
#include <scenes/scene.hpp>
#include <text/font.hpp>
#include <sounds/soundmanager.hpp>
#include <events/handler.hpp>

namespace sq {

class Application {
public:
    Application(glm::uvec2 _size, bool _showfps, bool _vsync,
                bool _resizable, const std::string& _title);

    FT_Library ftLib;
    SoundManager soundManager;

    int run();
    void quit(int _code);

    void set_size(glm::uvec2);
    void set_vsync(bool _vsync);
    void set_showfps(bool _showfps);

    glm::uvec2 get_size();
    bool get_vsync();
    bool get_showfps();

    void sweep_handler(const std::string& _id);
    void sweep_scene(const std::string& _id);

    IndexedMap<std::string, Scene::Ptr> sceneIM;
    IndexedMap<std::string, Handler::Ptr> handlerIM;

protected:
    sf::Window window;
    int retCode;

    std::set<std::string> handlerSweep;
    std::set<std::string> sceneSweep;

    bool vsync;
    bool showfps;
    std::string title;
};

}
