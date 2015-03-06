#pragma once
#include "forward.hpp"

#include <memory>
#include <set>

#include <SFML/Window/Window.hpp>

#include "app/settings.hpp"
#include "events/handler.hpp"
#include "gl/preprocessor.hpp"
#include "misc/indexedmap.hpp"
#include "scenes/scene.hpp"
#include "scripts/chaiscript.hpp"
#include "sounds/soundmanager.hpp"

namespace sq {

class Application : NonCopyable {
public:
    Application(glm::uvec2 _size, bool _resizable);

    int run();
    void quit(int _code);

    void set_size(glm::uvec2 _size);
    glm::uvec2 get_size();

    void sweep_handler(const string& _id);
    void sweep_scene(const string& _id);

    IndexedMap<string, std::unique_ptr<Scene>> sceneIM;
    IndexedMap<string, std::unique_ptr<Handler>> handlerIM;

    chai::ChaiScript cs;
    SoundManager sndMan;
    SettingMap settings;
    Preprocessor preproc;

protected:
    sf::Window window;
    int retCode;

    std::set<string> handlerSweep;
    std::set<string> sceneSweep;

    void update_settings();
};

}
