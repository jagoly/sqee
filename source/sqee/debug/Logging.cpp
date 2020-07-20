#include <sqee/debug/Logging.hpp>

#include <sqee/macros.hpp>

#include <ctime>

using namespace sq;

//============================================================================//

String sq::get_time_string()
{
    // todo: c++20 has a bunch of nice new time stuff

    auto now = std::time(nullptr);

    DISABLE_WARNING_MSVC_DEPRECIATED;
    auto local = *std::localtime(&now);
    ENABLE_WARNING_MSVC_DEPRECIATED;

    char buf[10];
    std::strftime(buf, 10, "%H:%M:%S", &local);

    return buf;
}
