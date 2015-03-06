#include <cctype>

#include "app/application.hpp"
#include "app/logging.hpp"
#include "scripts/chaiscript.hpp"
#include "text/text.hpp"

#include "scripts/console.hpp"

using namespace sq;

namespace Action {
enum { Up, Right, Down, Left, Backspace, Delete, Home, End, Return, ShiftReturn };
}

void SceneConsole::update() {}

void SceneConsole::render(float _ft) {
    if (active) {
        sq::draw_tiny_text(pretty_print(), 5, Alignment::TL, {8, 10}, app.get_size());
    }
}
void SceneConsole::resize(glm::uvec2 _size) {}

void SceneConsole::toggle_active() {
    static bool prevKR = false;
    active = !active;
    if (active) {
        prevKR = app.settings.crnt<bool>("app_keyrepeat");
        app.settings.mod<bool>("app_keyrepeat", true);
        app.settings.apply();
    } else {
        app.settings.mod<bool>("app_keyrepeat", prevKR);
        app.settings.apply();
    }
}

void SceneConsole::exec() {
    try {
        app.cs.eval(inputStr);
    } catch (chai::exception::eval_error &err) {
        std::cout << err.pretty_print() << std::endl;
    }

    inputStr.clear();
    curPos = 0;
}

void SceneConsole::handle_character(char _c) {
    if (std::isprint(_c)) // all printable chars
        inputStr.insert(curPos++, 1, _c);
}

void SceneConsole::handle_action(int _action) {
    if (_action == Action::Up) {

    } else
    if (_action == Action::Right) {
        if (curPos < inputStr.length())
            curPos++;
    } else
    if (_action == Action::Down) {

    } else
    if (_action == Action::Left) {
        if (curPos > 0)
            curPos--;
    } else
    if (_action == Action::Backspace) {
        if (!inputStr.empty() && curPos > 0)
            inputStr.erase(--curPos, 1);
    } else
    if (_action == Action::Delete) {
        if (inputStr.length() > curPos)
            inputStr.erase(curPos, 1);
    } else
    if (_action == Action::Home) {
        curPos = 0;
    } else
    if (_action == Action::End) {
        curPos = inputStr.length();
    } else
    if (_action == Action::ShiftReturn) {
        inputStr.insert(curPos++, 1, '\n');
    } else
    if (_action == Action::Return) {
        exec();
    }
}

string SceneConsole::pretty_print() {
    string ppStr = inputStr;
    size_t pos = 0;
    while ((pos = ppStr.find("\n", pos)) != string::npos) {
        ppStr.replace(pos, 1, "\n--> ");
        pos += 4;
    }
    return ">>> " + ppStr;
}

bool HandlerConsole::handle(const sf::Event& _event) {
    static SceneConsole* console = static_cast<SceneConsole*>(app.sceneIM.get("console").get());

    if (_event.type == sf::Event::KeyPressed && _event.key.code == sf::Keyboard::Menu) {
        console->toggle_active();
        return true;
    }

    if (!console->active) return false;

    if (_event.type == sf::Event::KeyPressed) {
        if (_event.key.code == sf::Keyboard::Up)
            console->handle_action(Action::Up);
        else if (_event.key.code == sf::Keyboard::Right)
            console->handle_action(Action::Right);
        else if (_event.key.code == sf::Keyboard::Down)
            console->handle_action(Action::Down);
        else if (_event.key.code == sf::Keyboard::Left)
            console->handle_action(Action::Left);
        else if (_event.key.code == sf::Keyboard::BackSpace)
            console->handle_action(Action::Backspace);
        else if (_event.key.code == sf::Keyboard::Delete)
            console->handle_action(Action::Delete);
        else if (_event.key.code == sf::Keyboard::Home)
            console->handle_action(Action::Home);
        else if (_event.key.code == sf::Keyboard::End)
            console->handle_action(Action::End);
        else if (_event.key.code == sf::Keyboard::Return)
            console->handle_action(_event.key.shift ? Action::ShiftReturn : Action::Return);
        return true; // catch all keyboard input
    } else if (_event.type == sf::Event::KeyReleased) return true;

    if (_event.type == sf::Event::TextEntered) {
        if (_event.text.unicode < 128) // ascii only
            console->handle_character(static_cast<char>(_event.text.unicode));
        return true;
    }

    return false;
}
