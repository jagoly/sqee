#include <cctype>

#include "app/application.hpp"
#include "app/logging.hpp"
#include "gl/maths.hpp"
#include "scripts/chaiscript.hpp"
#include "text/text.hpp"

#include "scripts/console.hpp"

using namespace sq;

void SceneConsole::update() {}

void SceneConsole::render(float _ft) {
    if (active) {
        sq::draw_tiny_text(pretty_print(), 5, Alignment::TL, {8, 10}, app.get_size());
    }
}
void SceneConsole::resize(glm::uvec2 _size) {}

void SceneConsole::exec() {
    try {
        app.cs.eval(inputStr);
    } catch (chai::exception::eval_error &err) {
        std::cout << err.pretty_print() << std::endl;
    }

    inputStr.clear();
    curPos = 0;
}

void SceneConsole::handle_char(char _c) {
    if (_c == 8) { // backspace
        if (!inputStr.empty() && curPos > 0)
            inputStr.erase(--curPos, 1);
    } else if (_c == 127) { // delete
        if (inputStr.length() > curPos)
            inputStr.erase(curPos, 1);
    } else if (_c == -2) { // right
        if (curPos < inputStr.length())
            curPos++;
    } else if (_c == -4) { // left
        if (curPos > 0)
            curPos--;
    } else if (std::isprint(_c) || _c == 10) { // all others inc newline
        inputStr.insert(curPos++, 1, _c);
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

    if (!console->active) {
        if (_event.type == sf::Event::KeyPressed && _event.key.code == sf::Keyboard::Menu) {
            console->active = true;
            return true;
        }
        return false;
    }

    if (_event.type == sf::Event::KeyPressed) {
        if (_event.key.code == sf::Keyboard::Return) {
            if (_event.key.shift) console->handle_char(10);
            else console->exec();
        }
        else if (_event.key.code == sf::Keyboard::Menu)
            console->active = false;
        else if (_event.key.code == sf::Keyboard::Up)
            console->handle_char(-1);
        else if (_event.key.code == sf::Keyboard::Right)
            console->handle_char(-2);
        else if (_event.key.code == sf::Keyboard::Down)
            console->handle_char(-3);
        else if (_event.key.code == sf::Keyboard::Left)
            console->handle_char(-4);
        return true; // catch all keyboard input
    } else if (_event.type == sf::Event::KeyReleased) return true;

    if (_event.type == sf::Event::TextEntered) {
        if (_event.text.unicode < 128) // ascii only
            console->handle_char(static_cast<char>(_event.text.unicode));
        return true;
    }
    return false;
}
