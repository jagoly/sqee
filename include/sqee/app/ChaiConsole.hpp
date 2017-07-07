#pragma once

#include <forward_list>

#include <sqee/builtins.hpp>

#include <sqee/app/Scene.hpp>
#include <sqee/app/ChaiEngine.hpp>
#include <sqee/app/Event.hpp>

//============================================================================//

namespace sq {

//============================================================================//

/// The SQEE ChaiScript Console.
class ChaiConsole final : public Scene
{
public: //====================================================//

    /// Constructor.
    ChaiConsole(ChaiEngine& engine);

    //--------------------------------------------------------//

    /// Call to handle console input.
    void handle_event(Event event);

    //--------------------------------------------------------//

    /// Toggle the console.
    void toggle_active() { mActive = !mActive; }

    /// Check if the console is active,
    bool check_active() const { return mActive; }

    //--------------------------------------------------------//

    /// Clear cnsole output.
    void clear();

    /// Display command history.
    void history();

    /// Print to the console.
    void print(const string& arg);

private: //===================================================//

    void update() override;

    void render(double elapsed) override;

    //--------------------------------------------------------//

    ChaiEngine& mEngine;

    //--------------------------------------------------------//

    string mInput;

    std::forward_list<string> mOutput;
    std::vector<string> mHistory;

    int mHistoryIndex = -1;
    uint mCursorPos = 0u;

    bool mCursorVisible = false;
    bool mCursorDelay = false;

    bool mActive = false;
};

//============================================================================//

} // namespace sq
