// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#include <sqee/app/ChaiConsole.hpp>

#include <chaiscript/chaiscript.hpp>

#include <sqee/debug/Text.hpp>

using namespace sq;

//============================================================================//

ChaiConsole::ChaiConsole(ChaiEngine& engine) : Scene(0.5), mEngine(engine) {}

//============================================================================//

void ChaiConsole::update()
{
    mCursorVisible = !mCursorVisible;
    mCursorVisible |= mCursorDelay;

    mCursorDelay = false;
}

//============================================================================//

void ChaiConsole::render(double)
{
    if (mActive == true)
    {
        String outStr = ">>> " + mInput;
        String::size_type pos = 0u;

        const char cursor = mCursorVisible ? char(5) : ' ';
        outStr.insert(outStr.begin() + mCursorPos + 4u, cursor);

        while ((pos = outStr.find("\n", pos)) != String::npos)
        { outStr.replace(pos, 1u, "\n--> "); pos += 4u; }

        for (const auto& str : mOutput) outStr.append('\n' + str);

        // render output text to the top-left of the view port
        render_text_basic(outStr, Vec2I(+1, -1), Vec2I(-1, +1), Vec2F(24, 30), Vec4F(1, 1, 1, 1), true);
    }
}

//============================================================================//

void ChaiConsole::handle_event(Event event)
{
    using Type = Event::Type;
    using Key = Keyboard_Key;

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

            try { mEngine.eval(mInput); }
            catch (const chai::exception::eval_error& error)
            { mOutput.emplace_front(error.what()); }

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

void ChaiConsole::clear()
{
    mOutput.clear();
}

void ChaiConsole::history()
{
    for (const String& command : mHistory)
    {
        mOutput.emplace_front("> " + command);
    }
}

void ChaiConsole::print(String arg)
{
    mOutput.emplace_front(arg);
}
