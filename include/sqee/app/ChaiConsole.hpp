#pragma once

#include <forward_list>

#include <sqee/app/Application.hpp>

namespace sq {

//============================================================================//

/// The SQEE Chaiscript Console
class ChaiConsole final : NonCopyable
{
public:

    //========================================================//

    /// Constructor
    ChaiConsole(Application& app);

    //========================================================//

    /// called to handle text input
    void handle(sf::Event event);

    /// called two times per second
    void tick();

    /// called once every frame
    void render();

    //========================================================//

    /// enable or disable the console
    void toggle_active();

    /// check if the console is active
    bool is_active() const { return mActive; }

    //========================================================//

    void cs_print(const string& str);
    void cs_history();
    void cs_clear();

    //========================================================//

    float accumulation = 0.0f;

private:

    //========================================================//

    Application& mApplication;

    string mInput;

    std::forward_list<string> mOutput;
    std::vector<string> mHistory;

    int mHistoryIndex = -1;
    uint mCursorPos = 0u;

    bool mCursorVisible = false;
    bool mCursorDelay = false;

    bool mActive = false;
};

}
