#pragma once

#include <forward_list>

#include <sqee/misc/Builtins.hpp>
#include <sqee/app/Scene.hpp>
#include <sqee/redist/sol.hpp>

namespace sqt {

//============================================================================//

class LuaConsole final : public sq::Scene
{
public: //====================================================//

    LuaConsole(sol::state& lua);

    void handle_event(sq::Event event) override;

    void toggle_active() { mActive = !mActive; }

    bool check_active() const { return mActive; }

    void clear();

    void history();

    void print(String arg);

private: //===================================================//

    void update() override;

    void render(double elapsed) override;

    sol::state& mLua;

    String mInput;

    std::forward_list<String> mOutput;
    Vector<String> mHistory;

    int mHistoryIndex = -1;
    uint mCursorPos = 0u;

    bool mCursorVisible = false;
    bool mCursorDelay = false;

    bool mActive = false;
};

//============================================================================//

} // namespace sqt
