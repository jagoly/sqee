#include "sqee/app/application.hpp"
#include "sqee/app/settings.hpp"
#include "sqee/app/logging.hpp"
#include "sqee/scripts/chaiscript.hpp"
#include "sqee/text/text.hpp"
#include "sqee/scripts/scene.hpp"

using namespace sq;

SceneConsole::SceneConsole(Application* _app) : Scene(_app) {
    appBase->cs->add_global(chai::var(this), "console");
    settings->add<bool>("console_active", false);
}

void SceneConsole::render(float _ft) {
    if (active == true) {
        string ppStr = input; size_t pos = 0u;
        while ((pos = ppStr.find("\n", pos)) != string::npos)
            ppStr.replace(pos, 1u, "\n--> "), pos += 4u;
        string finalStr = ">>> " + ppStr;
        for (const auto& str : output) finalStr.append("\n"+str);
        sq::draw_tiny_text(finalStr, 3.f, Alignment::TL, {8, 10}, appBase->get_size());
    }
}

void SceneConsole::exec() {
    try { appBase->cs->eval(input); }
    catch (chai::exception::eval_error &err) {
        output.emplace_front(err.what());
    } input.clear(); curPos = 0u;
}

void SceneConsole::handle_character(char _c) {
    if (std::isprint(_c)) // all printable chars
        input.insert(curPos++, 1u, _c);
}

void SceneConsole::handle_action(Action _action) {
    static int histInd = -1;
    if (_action == Action::Return) {
        if ((!history.size() || history.back() != input) && input != "")
            history.emplace_back(input);
        histInd = -1; exec();
    }
    else if (_action == Action::ShiftReturn) {
        input.insert(curPos++, 1u, '\n');
    }
    else if (_action == Action::Up) {
        if (history.size()) {
            if (histInd == -1) histInd = history.size()-1;
            else if (histInd > 0) histInd--;
            input = history[histInd]; curPos = input.length();
        }
    }
    else if (_action == Action::Down) {
        if (history.size()) {
            if (histInd == -1) histInd = history.size()-1;
            else if (histInd < int(history.size()-1)) histInd++;
            input = history[histInd]; curPos = input.length();
        }
    }
    else if (_action == Action::Left) {
        if (curPos > 0u)
            curPos--;
    }
    else if (_action == Action::Right) {
        if (curPos < input.length())
            curPos++;
    }

    else if (_action == Action::Backspace) {
        if (!input.empty() && curPos > 0u)
            input.erase(--curPos, 1u);
    }
    else if (_action == Action::Delete) {
        if (input.length() > curPos)
            input.erase(curPos, 1u);
    }
    else if (_action == Action::Home) {
        curPos = 0u;
    }
    else if (_action == Action::End) {
        curPos = input.length();
    }
}

void SceneConsole::cs_print(const string& _value) {
    output.emplace_front(_value);
}

void SceneConsole::cs_clear() {
    output.clear();
}

void SceneConsole::cs_history() {
    for (const auto& cmd : history)
        output.emplace_front("> "+cmd);
}
