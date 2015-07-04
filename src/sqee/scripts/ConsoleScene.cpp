#include <sqee/app/Application.hpp>
#include <sqee/app/SettingsMap.hpp>
#include <sqee/app/Logging.hpp>
#include <sqee/scripts/ChaiScript.hpp>
#include <sqee/text/Text.hpp>
#include <sqee/scripts/ConsoleScene.hpp>

using namespace sq;

SceneConsole::SceneConsole(Application* _app) : Scene(_app) {
    appBase->cs->add_global(chai::var(this), "console");
    settings->add<bool>("console_active", false);
    tickRate = 2u;
}

void SceneConsole::update() {
    if (active == false) return;

    if (tickDelay == false)
        tickSwitch = !tickSwitch;
    else tickDelay = false;
}

void SceneConsole::render(float _ft) {
    if (active == false) return;

    static const TextBasic tb = {
        TextBasic::Flow::Positive, TextBasic::Flow::Negative,
        TextBasic::Align::Negative, TextBasic::Align::Positive,
        fvec3(1.f, 1.f, 1.f), fvec2(24.f, 30.f), true
    };

    string outStr = input; size_t pos = 0u; outStr.insert(0, ">>> ");
    outStr.insert(outStr.begin()+curPos+4, tickSwitch ? char(5) : ' ');
    while ((pos = outStr.find("\n", pos)) != string::npos)
        outStr.replace(pos, 1u, "\n--> "), pos += 4u;
    for (const auto& str : output) outStr.append('\n' + str);
    sq::render_text_basic(outStr, tb, 1.f, appBase->get_size());
}

void SceneConsole::exec() {
    try { appBase->cs->eval(input); }
    catch (chai::exception::eval_error& err) {
        output.emplace_front(err.what());
    } input.clear(); curPos = 0u;
}

void SceneConsole::handle_character(char _c) {
    if (std::isprint(_c)) // all printable chars
        input.insert(curPos++, 1u, _c);

    tickSwitch = true;
    tickDelay = true;
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

    tickSwitch = true;
    tickDelay = true;
}

void SceneConsole::cs_print(const string& _value) {
    output.emplace_front(_value);
}

void SceneConsole::cs_history() {
    for (const auto& cmd : history)
        output.emplace_front("> "+cmd);
}

void SceneConsole::cs_clear() {
    output.clear();
}
