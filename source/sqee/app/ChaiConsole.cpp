#include <SFML/Window/Event.hpp>

#include <sqee/messages.hpp>
#include <sqee/gl/Context.hpp>
#include <sqee/debug/Text.hpp>

#include <sqee/scripts/ChaiEngine.hpp>

#include <sqee/app/MessageBus.hpp>
#include <sqee/app/ChaiConsole.hpp>

using namespace sq;

//============================================================================//

ChaiConsole::ChaiConsole(Application& app) : mApplication(app) {}

//============================================================================//

void ChaiConsole::handle(sf::Event event)
{
    //========================================================//

    if (event.type == sf::Event::TextEntered)
    {
        const uint ucode = event.text.unicode;

        if (ucode < 128u && std::isprint(char(ucode)))
        {
            mInput.insert(mCursorPos++, 1u, char(ucode));

            mCursorVisible = mCursorDelay = true;
        }
    }

    //========================================================//

    if (event.type == sf::Event::KeyPressed)
    {
        //========================================================//

        if (event.key.code == sf::Keyboard::Return)
        {
            if (event.key.shift == true)
                mInput.insert(mCursorPos++, 1u, '\n');

            else if (mInput.empty() == false)
            {
                if (mHistory.empty() || mHistory.back() != mInput)
                {
                    mHistory.emplace_back(mInput);
                    mHistoryIndex = -1;
                }

                try { mApplication.get_chai_engine().eval(mInput); }
                catch (chai::exception::eval_error& error)
                { mOutput.emplace_front(error.what()); }

                mCursorPos = 0u; mInput.clear();
            }
        }

        //========================================================//

        if (event.key.code == sf::Keyboard::Up)
        {
            if (mHistory.empty() == false)
            {
                if (mHistoryIndex == -1) --(mHistoryIndex = mHistory.size());
                else if (mHistoryIndex > 0) --mHistoryIndex;

                mInput = mHistory[size_t(mHistoryIndex)];
                mCursorPos = mInput.length();
            }
        }

        //========================================================//

        if (event.key.code == sf::Keyboard::Down)
        {
            if (mHistory.empty() == false)
            {
                if (mHistoryIndex == -1) --(mHistoryIndex = mHistory.size());
                else if (mHistoryIndex < int(mHistory.size())-1) ++mHistoryIndex;

                mInput = mHistory[size_t(mHistoryIndex)];
                mCursorPos = mInput.length();
            }
        }

        //========================================================//

        if (event.key.code == sf::Keyboard::Left)
            if (mCursorPos > 0u) --mCursorPos;

        if (event.key.code == sf::Keyboard::Right)
            if (mCursorPos < mInput.length()) ++mCursorPos;

        //========================================================//

        if (event.key.code == sf::Keyboard::BackSpace)
            if (mInput.empty() == false && mCursorPos > 0u)
                mInput.erase(--mCursorPos, 1u);

        if (event.key.code == sf::Keyboard::Delete)
            if (mInput.length() > mCursorPos)
                mInput.erase(mCursorPos, 1u);

        //========================================================//

        if (event.key.code == sf::Keyboard::End)
            mCursorPos = mInput.length();

        if (event.key.code == sf::Keyboard::Home)
            mCursorPos = 0u;

        //========================================================//

        mCursorVisible = mCursorDelay = true;
    }
}

//============================================================================//

void ChaiConsole::tick()
{
    mCursorVisible = !mCursorVisible;
    mCursorVisible |= mCursorDelay;
    mCursorDelay = false;
}

//============================================================================//

void ChaiConsole::render()
{
    static auto& context = Context::get();

    context.set_ViewPort(mApplication.OPTION_WindowSize);

    string outStr = ">>> " + mInput;
    string::size_type pos = 0u;

    const char cursor = mCursorVisible ? char(5) : ' ';
    outStr.insert(outStr.begin() + mCursorPos + 4u, cursor);

    while ((pos = outStr.find("\n", pos)) != string::npos)
    { outStr.replace(pos, 1u, "\n--> "); pos += 4u; }

    for (const auto& str : mOutput) outStr.append('\n' + str);

    // render output text to the top-left of the screen
    render_text_basic(outStr, Vec2I(+1, -1), Vec2I(-1, +1), Vec2F(24, 30), Vec4F(1, 1, 1, 1), true);
}

//============================================================================//

void ChaiConsole::toggle_active()
{
    mActive = !mActive;

    const msg::Toggle_Chai_Console message { mActive };
    mApplication.get_message_bus().send_message(message);
}

//============================================================================//

void ChaiConsole::cs_print(const string& str)
{
    mOutput.emplace_front(str);
}

void ChaiConsole::cs_history()
{
    for (const auto& cmd : mHistory)
        mOutput.emplace_front("> " + cmd);
}

void ChaiConsole::cs_clear()
{
    mOutput.clear();
}
