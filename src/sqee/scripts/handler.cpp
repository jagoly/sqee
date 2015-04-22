#include "sqee/app/application.hpp"
#include "sqee/scripts/scene.hpp"
#include "sqee/scripts/handler.hpp"

using namespace sq;

bool HandlerConsole::handle(const sf::Event& _event) {
    static SceneConsole& scene = *static_cast<SceneConsole*>(app.sceneIM.get("console").get());
    using sfkb = sf::Keyboard::Key; using Act = SceneConsole::Action;

    if (_event.type == sf::Event::KeyPressed && _event.key.code == sfkb::Menu) {
        if (scene.active) deactivate(); else activate(); return true;
    } if (!scene.active) return false;

    if (_event.type == sf::Event::KeyPressed) {
        if (_event.key.code == sfkb::Return)
            scene.handle_action( _event.key.shift ? Act::ShiftReturn : Act::Return);
        else if (_event.key.code == sfkb::Up)        scene.handle_action(Act::Up);
        else if (_event.key.code == sfkb::Right)     scene.handle_action(Act::Right);
        else if (_event.key.code == sfkb::Down)      scene.handle_action(Act::Down);
        else if (_event.key.code == sfkb::Left)      scene.handle_action(Act::Left);
        else if (_event.key.code == sfkb::BackSpace) scene.handle_action(Act::Backspace);
        else if (_event.key.code == sfkb::Delete)    scene.handle_action(Act::Delete);
        else if (_event.key.code == sfkb::Home)      scene.handle_action(Act::Home);
        else if (_event.key.code == sfkb::End)       scene.handle_action(Act::End);
        return true; // catch all keyboard input
    } else if (_event.type == sf::Event::KeyReleased) return true;

    if (_event.type == sf::Event::TextEntered) {
        if (_event.text.unicode < 128) // ascii only
            scene.handle_character(static_cast<char>(_event.text.unicode));
        return true;
    }

    return false;
}

void HandlerConsole::activate() {
    static SceneConsole& scene = *static_cast<SceneConsole*>(app.sceneIM.get("console").get());
    app.settings.mod<bool>("app_keyrepeat", true);
    app.settings.apply();
    scene.active = true;
}

void HandlerConsole::deactivate() {
    static SceneConsole& scene = *static_cast<SceneConsole*>(app.sceneIM.get("console").get());
    app.settings.mod<bool>("app_keyrepeat", false);
    app.settings.apply();
    scene.active = false;
}
