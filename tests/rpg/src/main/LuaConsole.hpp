#pragma once

#include <sqee/setup.hpp>

#include <sqee/app/Scene.hpp>
#include <sqee/app/WrenPlus.hpp>
#include <sqee/core/Types.hpp>

#include <forward_list>

namespace sqt {

//============================================================================//

class WrenConsole final : public sq::Scene
{
public: //====================================================//

    WrenConsole(wren::WrenPlusVM& vm);

    void handle_event(sq::Event event) override;

    void toggle_active() { mActive = !mActive; }

    bool check_active() const { return mActive; }

    void clear();

    void history();

    void print(std::string arg);

private: //===================================================//

    void update() override;

    void render(double elapsed) override;

    wren::WrenPlusVM& mWrenVM;

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
