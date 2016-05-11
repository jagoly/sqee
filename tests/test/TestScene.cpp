/*#include <SFML/Window/Event.hpp>

#include <JagolyGUI/containers/BoxH.hpp>
#include <JagolyGUI/containers/BoxV.hpp>
#include <JagolyGUI/widgets/Button.hpp>
#include <JagolyGUI/widgets/Label.hpp>
#include <JagolyGUI/Helpers.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/gl/Drawing.hpp>

#include "TestScene.hpp"

using namespace sqt;
namespace maths = sq::maths;

TestScene::TestScene(sq::Application* _app) : sq::Scene(_app) {
    root.minimumSize = {200u, 500u};
    root.maximumSize = {200u, 500u};

    inputHandler.root = &root;

    auto m = root.set<gui::BoxV>();
    //m->maximumSize = {100u, 250u};

    auto wA = m->add_back<gui::Widget>("A");
    wA->maximumSize.x = 100u;
    wA->maximumSize.y = 20u;

    auto wB = m->add_back<gui::Widget>("B");
    wB->maximumSize.x = 100u;
    wB->maximumSize.y = 40u;

    auto wC = m->add_back<gui::BoxH>("C");
    wC->maximumSize.y = 300u;

    auto wCA = wC->add_back<gui::BoxV>("A");
    auto wCAA = wCA->add_back<gui::Button>("A");
    auto wCAB = wCA->add_back<gui::Widget>("B");
    auto wCAC = wCA->add_back<gui::Button>("C");
    auto wCAD = wCA->add_back<gui::Widget>("D");
    auto wCB = wC->add_back<gui::Widget>("B");

    auto wD = m->add_back<gui::Widget>("D");
    wD->maximumSize.x = 100u;
    wD->maximumSize.y = 30u;
}


void TestScene::update() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) root.minimumSize.y += 1u, root.maximumSize.y += 1u;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) root.minimumSize.y -= 1u, root.maximumSize.y -= 1u;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) root.minimumSize.x += 1u, root.maximumSize.x += 1u;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) root.minimumSize.x -= 1u, root.maximumSize.x -= 1u;

    root.calculatedSize = root.update_required_size();
    root.calculatedPos = {200u, 20u};
    root.update_layout_expanding();
    root.update_layout_positions();

    auto m = root.get<gui::BoxV>();
    sq::log_only("%f", m->get("A")->calculatedPos.y);
    sq::log_only("%f", m->get("B")->calculatedPos.y);
    sq::log_only("%f", m->get("C")->calculatedPos.y);
    sq::log_only("%f", m->get("D")->calculatedPos.y);
    std::cout << std::endl;
}


void TestScene::render() {
    sq::CLEAR_COLOR_DEPTH_STENC();

    uiRenderer.render_gui(&root);
}


void TestScene::refresh() {
}

bool TestScene::handle(const sf::Event& _event) {
    if (_event.type == sf::Event::MouseMoved) {
        inputHandler.handle_MouseMoved(gui::Vec2U{_event.mouseMove.x, 600u - _event.mouseMove.y});
    } else if (_event.type == sf::Event::MouseButtonPressed) {
        inputHandler.handle_ButtonPressed(gui::input::sfml_to_button(_event.mouseButton.button),
                                          gui::Vec2U{_event.mouseButton.x, 600u - _event.mouseButton.y});
    } else if (_event.type == sf::Event::MouseButtonReleased) {
        inputHandler.handle_ButtonReleased(gui::input::sfml_to_button(_event.mouseButton.button),
                                           gui::Vec2U{_event.mouseButton.x, 600u - _event.mouseButton.y});
    }

    return false;
}
*/
