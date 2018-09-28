// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <forward_list>

#include <sqee/misc/Builtins.hpp>

#include <sqee/app/ChaiEngine.hpp>
#include <sqee/app/Scene.hpp>

namespace sq {

//============================================================================//

/// The SQEE ChaiScript Console.
class SQEE_API ChaiConsole final : public Scene
{
public: //====================================================//

    /// Constructor.
    ChaiConsole(ChaiEngine& engine);

    //--------------------------------------------------------//

    /// Call to handle console input.
    void handle_event(Event event) override;

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
    void print(String arg);

private: //===================================================//

    void update() override;

    void render(double elapsed) override;

    //--------------------------------------------------------//

    ChaiEngine& mEngine;

    //--------------------------------------------------------//

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

} // namespace sq
