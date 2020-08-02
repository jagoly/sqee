#include "LuaConsole.hpp"

#include <sqee/app/Event.hpp>
#include <sqee/debug/Text.hpp>

using namespace sqt;

//============================================================================//

LuaConsole::LuaConsole(sol::state& lua) : sq::Scene(0.5), mLua(lua) {}

//============================================================================//

void LuaConsole::update()
{
    mCursorVisible = !mCursorVisible;
    mCursorVisible |= mCursorDelay;

    mCursorDelay = false;
}

//============================================================================//

void LuaConsole::render(double)
{
    if (mActive == true)
    {
        std::string outStr = ">>> " + mInput;
        std::string::size_type pos = 0u;

        const char cursor = mCursorVisible ? char(5) : ' ';
        outStr.insert(outStr.begin() + mCursorPos + 4u, cursor);

        while ((pos = outStr.find("\n", pos)) != std::string::npos)
        { outStr.replace(pos, 1u, "\n--> "); pos += 4u; }

        for (const auto& str : mOutput) outStr.append('\n' + str);

        // render output text to the top-left of the view port
        sq::render_text_basic(outStr, Vec2I(+1, -1), Vec2I(-1, +1), Vec2F(24, 30), Vec4F(1, 1, 1, 1), true);
    }
}

//============================================================================//

void LuaConsole::handle_event(sq::Event event)
{
    using Type = sq::Event::Type;
    using Key = sq::Keyboard_Key;

    //--------------------------------------------------------//

    // enter a text character /////

    if (event.type == Type::Text_Entry)
    {
        const uint32_t code = event.data.text.unicode;

        if (code < 128u && std::isprint(char(code)))
        {
            mInput.insert(mCursorPos++, 1u, char(code));

            mCursorVisible = mCursorDelay = true;
        }
    }

    //--------------------------------------------------------//

    if (event.type == Type::Keyboard_Press)
    {
        mCursorVisible = mCursorDelay = true;

        const auto& data = event.data.keyboard;

        //--------------------------------------------------------//

        // insert a new line /////

        if (data.key == Key::Return && data.shift)
        {
            mInput.insert(mCursorPos++, 1u, '\n');
        }

        //--------------------------------------------------------//

        // submit the command /////

        if (data.key == Key::Return && !data.shift && !mInput.empty())
        {
            if (mHistory.empty() || mHistory.back() != mInput)
            {
                mHistory.emplace_back(mInput);
                mHistoryIndex = -1;
            }

//            try { mEngine.eval(mInput); }
//            catch (const chai::exception::eval_error& error)
//            { mOutput.emplace_front(error.what()); }

            mCursorPos = 0u;
            mInput.clear();
        }

        //--------------------------------------------------------//

        // switch to the previous history entry /////

        if (data.key == Key::Arrow_Up && !mHistory.empty())
        {
            if (mHistoryIndex == -1) mHistoryIndex = int(mHistory.size()) - 1;
            else if (mHistoryIndex > 0) --mHistoryIndex;

            mInput = mHistory[size_t(mHistoryIndex)];
            mCursorPos = uint(mInput.length());
        }

        //--------------------------------------------------------//

        // switch to the next history entry /////

        if (data.key == Key::Arrow_Down && !mHistory.empty())
        {
            if (mHistoryIndex == -1) mHistoryIndex = int(mHistory.size()) - 1;
            else if (mHistoryIndex < int(mHistory.size()) - 1) ++mHistoryIndex;

            mInput = mHistory[size_t(mHistoryIndex)];
            mCursorPos = uint(mInput.length());
        }

        //--------------------------------------------------------//

        // backspace or delete a character /////

        if (data.key == Key::BackSpace && !mInput.empty())
        {
            if (mCursorPos > 0u) mInput.erase(--mCursorPos, 1u);
        }

        if (data.key == Key::Delete && !mInput.empty())
        {
            if (mCursorPos < mInput.length()) mInput.erase(mCursorPos, 1u);
        }

        //--------------------------------------------------------//

        // move the cursor left or right /////

        if (data.key == Key::Arrow_Left && mCursorPos > 0u) --mCursorPos;

        if (data.key == Key::Arrow_Right && mCursorPos < mInput.length()) ++mCursorPos;

        //--------------------------------------------------------//

        // move the cursor to the line start or end /////

        if (data.key == Key::Home) mCursorPos = 0u;

        if (data.key == Key::End) mCursorPos = uint(mInput.length());

        //--------------------------------------------------------//
    }
}

//============================================================================//

void LuaConsole::clear()
{
    mOutput.clear();
}

void LuaConsole::history()
{
    for (const std::string& command : mHistory)
    {
        mOutput.emplace_front("> " + command);
    }
}

void LuaConsole::print(std::string arg)
{
    mOutput.emplace_front(arg);
}
