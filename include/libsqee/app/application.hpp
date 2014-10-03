#pragma once

#include <list>
#include <forward_list>
#include <set>
#include <memory>

#include <gl/gl.hpp>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include <scenes/scene.hpp>
#include <gl/textures.hpp>
#include <text/font.hpp>
#include <sounds/soundmanager.hpp>
#include <events/handler.hpp>

namespace sq {

class Application {
public:
    Application(glm::uvec2 size, bool _showfps,
                bool _vsync, bool _resizable, std::string _title);

    bool running;

    FT_Library ftLib;
    SoundManager soundManager;

    void run();

    void set_size(glm::uvec2);
    glm::uvec2 get_size();

    void set_vsync(bool _vsync);
    bool get_vsync();

    void set_showfps(bool _showfps);
    bool get_showfps();

    void attach_handler(std::string strId, HandlerPtr handler);

    void append_scene(std::string strId, ScenePtr scene);
    void prepend_scene(std::string strId, ScenePtr scene);
    void insert_scene(int index, std::string strId, ScenePtr scene);

    Scene& get_scene_first();
    Scene& get_scene_last();
    Scene& get_scene(std::string strId);

    void sweep_handler(std::string strId);
    void sweep_scene(std::string strId);

protected:
    sf::Window* window;

    std::map<std::string, HandlerPtr> handlerMap;
    std::forward_list<HandlerPtr> handlerFList;
    std::map<std::string, ScenePtr> sceneMap;
    std::list<ScenePtr> sceneList;

    std::set<std::string> handlerSweep;
    std::set<std::string> sceneSweep;

    bool vsync;
    bool showfps;
    std::string title;
};

}
