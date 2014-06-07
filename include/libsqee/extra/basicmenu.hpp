#pragma once

#include <SFGUI/SFGUI.hpp>

#include <application.hpp>
#include <scene.hpp>

namespace sqe {

class SceneMenu : public sq::Scene {
public:
    SceneMenu(sq::Application*);

    sfg::Desktop desktop;
    sfg::Window::Ptr rootWindow;

    void update();
    void render(sf::RenderTarget&, float);
};

}
