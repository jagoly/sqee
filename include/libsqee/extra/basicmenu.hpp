#pragma once

#include <SFGUI/SFGUI.hpp>

#include <application.hpp>
#include <scene.hpp>
#include <handler.hpp>

namespace sqe {

class SceneMenu : public sq::Scene {
public:
    SceneMenu(sq::Application*, bool);

    bool active;

    virtual void activate();
    virtual void deactivate();

    void update();
    void render(sf::RenderTarget&, float);
    bool handleEvent(sf::Event&);

protected:
    sfg::Desktop desktop;
    sfg::Window::Ptr rootWindow;
};

class HandlerMenu : public sq::Handler {
public:
    HandlerMenu(sq::Application*, std::string);
    bool handle(sf::Event&);
protected:
    std::string menuId;
};

}
