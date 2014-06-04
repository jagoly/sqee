#pragma once

#include <SFGUI/SFGUI.hpp>

#include <libsqee/base.hpp>

namespace sqt {

class SceneMainMenu : public sq::Scene {
public:
    SceneMainMenu(sq::Application*);

    void update();
    void render(sf::RenderTarget&, float);
};

}
