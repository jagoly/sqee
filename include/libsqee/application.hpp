#pragma once

#include <list>
#include <forward_list>
#include <set>
#include <memory>

#include <extra/gl.hpp>
#include <SFGUI/SFGUI.hpp>

#include <scene.hpp>
#include <textureholder.hpp>
#include <fontholder.hpp>
#include <soundmanager.hpp>
#include <handler.hpp>

namespace sq {

class Application {
public:
    Application(unsigned int _width, unsigned int _height, unsigned int _AA, bool _showfps,
                bool _vsync, bool _resizable, std::string _title);

    bool running;

    TextureHolder texHolder;
    FontHolder fontHolder;
    SoundManager soundManager;

    sfg::SFGUI sfgui;

    void run();

    virtual void set_size(unsigned int _width, unsigned int _height);
    sf::Vector2u get_size();

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
    sf::RenderWindow* window;

    std::map<std::string, HandlerPtr> handlerMap;
    std::forward_list<HandlerPtr> handlerFList;
    std::map<std::string, ScenePtr> sceneMap;
    std::list<ScenePtr> sceneList;

    std::set<std::string> handlerSweep;
    std::set<std::string> sceneSweep;

    bool vsync;
    bool showfps;
};

}
