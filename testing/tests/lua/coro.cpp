#include "../../catch.hpp"
#include "../../Common.hpp"

#include <sqee/redist/sol.hpp>

//============================================================================//

void func_throwing()
{
    throw std::runtime_error("oops");
}

void func_yielding()
{
    return;
}

//static const char* const SCRIPT = R"lua(
//function scriptfun()

//end
//)lua";

TEST_CASE("Lua Coroutine", "[lua]")
{
    SECTION("cancel and restart")
    {
        sol::state lua;
        lua.open_libraries(sol::lib::base, sol::lib::coroutine);
    }
}
