#pragma once

#include <JagolyGUI/containers/Frame.hpp>
#include <JagolyGUI/text/FontFace.hpp>
#include <JagolyGUI/InputHandler.hpp>

#include <sqee/forward.hpp>
#include <sqee/builtins.hpp>
#include <sqee/scenes/Scene.hpp>
#include <sqee/handlers/Handler.hpp>
#include <sqee/maths/Vectors.hpp>

#include "UIRenderer.hpp"

namespace sqt {

namespace gui = jagolygui;

class TestScene : public sq::Scene {
public:
    TestScene(sq::Application* _app);

    void update();
    void render();
    void refresh();

    bool handle(const sf::Event& _event);

    gui::FontFace font;
    gui::Frame root;

    gui::InputHandler inputHandler;

    UIRenderer uiRenderer;

private:
};

}
