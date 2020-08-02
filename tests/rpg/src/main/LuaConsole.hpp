#pragma once

#include <sqee/setup.hpp>

#include <sqee/app/Scene.hpp>
#include <sqee/core/Types.hpp>
#include <sqee/redist/sol.hpp>

#include <forward_list>

using namespace sq::coretypes;

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

    void print(std::string arg);

private: //===================================================//

    void update() override;

    void render(double elapsed) override;

    sol::state& mLua;

    std::string mInput;

    std::forward_list<std::string> mOutput;
    std::vector<std::string> mHistory;

    int mHistoryIndex = -1;
    uint mCursorPos = 0u;

    bool mCursorVisible = false;
    bool mCursorDelay = false;

    bool mActive = false;
};

//============================================================================//

} // namespace sqt
