#include <SFML/Window/Event.hpp>

#include <sqee/debug/Logging.hpp>
#include <sqee/scripts/ChaiScript.hpp>
#include <sqee/app/Application.hpp>
#include <sqee/app/ChaiConsole.hpp>
#include <sqee/app/Settings.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/text/Text.hpp>

using namespace sq;

ChaiConsole::ChaiConsole(Application* _app) : app(_app) {}

void ChaiConsole::update() {
    if (cvisDelay == false)
        cursorVis = !cursorVis;
    else cvisDelay = false;
}

void ChaiConsole::render() {
    sq::VIEWPORT(app->get_size());
    string outStr = input; size_t pos = 0u; outStr.insert(0, ">>> ");
    outStr.insert(outStr.begin()+cursorPos+4, cursorVis ? char(5) : ' ');
    while ((pos = outStr.find("\n", pos)) != string::npos)
        outStr.replace(pos, 1u, "\n--> "), pos += 4u;
    for (const auto& str : output) outStr.append('\n' + str);

    render_text_basic(outStr, app->get_size(),
                      TextBasicFlow::Positive, TextBasicFlow::Negative,
                      TextBasicAlign::Negative, TextBasicAlign::Positive,
                      Vec2F(24.f, 30.f), Vec3F(1.f, 1.f, 1.f), 1.f, true);
}

void ChaiConsole::handle_input(sf::Event _event) {
    if (_event.type == sf::Event::TextEntered
            && _event.text.unicode < 128
            && std::isprint(char(_event.text.unicode))) {
        input.insert(cursorPos++, 1u, char(_event.text.unicode));
        cursorVis = true; cvisDelay = true;
    }

    if (_event.type == sf::Event::KeyPressed) {
        if (_event.key.code == sf::Keyboard::Return && _event.key.shift) {
            input.insert(cursorPos++, 1u, '\n');

        } else if (_event.key.code == sf::Keyboard::Return) {
            if (!input.empty() && (history.empty() || history.back() != input))
                history.emplace_back(input), histInd = -1;

            try { app->cs->eval(input); }
            catch (chai::exception::eval_error& err) {
                output.emplace_front(err.what()); }
            input.clear(); cursorPos = 0u;

        } else if (_event.key.code == sf::Keyboard::Up) {
            if (history.empty() == false) {
                if (histInd == -1) histInd = history.size() - 1;
                else if (histInd > 0) --histInd;
                cursorPos = (input = history[histInd]).length();
            }

        } else if (_event.key.code == sf::Keyboard::Down) {
            if (history.empty() == false) {
                if (histInd == -1) histInd = history.size() - 1;
                else if (histInd < int(history.size()-1)) ++histInd;
                cursorPos = (input = history[histInd]).length();
            }

        } else if (_event.key.code == sf::Keyboard::Left) {
            if (cursorPos > 0u) --cursorPos;

        } else if (_event.key.code == sf::Keyboard::Right) {
            if (cursorPos < input.length()) ++cursorPos;

        } else if (_event.key.code == sf::Keyboard::BackSpace) {
            if (!input.empty() && cursorPos > 0u)
                input.erase(--cursorPos, 1u);

        } else if (_event.key.code == sf::Keyboard::Delete) {
            if (input.length() > cursorPos)
                input.erase(cursorPos, 1u);

        } else if (_event.key.code == sf::Keyboard::Home) {
            cursorPos = 0u;

        } else if (_event.key.code == sf::Keyboard::End) {
            cursorPos = input.length();
        }

        cursorVis = true; cvisDelay = true;
    }
}

void ChaiConsole::toggle_active() {
    if ((active = !active)) {
        app->settings.mod<bool>("app_keyrepeat", true);
        for (auto func : onShowFuncs) func();
        app->configure();
    } else {
        app->settings.mod<bool>("app_keyrepeat", false);
        for (auto func : onHideFuncs) func();
        app->configure();
    }
}


void ChaiConsole::cs_print(const string& _value) {
    output.emplace_front(_value);
}

void ChaiConsole::cs_history() {
    for (const auto& cmd : history)
        output.emplace_front("> "+cmd);
}

void ChaiConsole::cs_clear() {
    output.clear();
}
