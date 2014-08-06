#pragma once

#include <application.hpp>
#include <scene.hpp>
#include <handler.hpp>

namespace sq {

class SceneMenu : public sq::Scene {
public:
    SceneMenu(sq::Application* _app, bool startActive);

    bool active;

    virtual void activate();
    virtual void deactivate();

    virtual void update();
    virtual void render(sf::RenderTarget& target, float ft);
    virtual bool handleEvent(sf::Event& event);

protected:
    sfg::Desktop desktop;
    sfg::Window::Ptr rootWindow;
};

class HandlerMenu : public sq::Handler {
public:
    HandlerMenu(sq::Application* _app, std::string strId);
    bool handle(sf::Event& event);
protected:
    std::string menuId;
};

}
