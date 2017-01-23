#include <ctime>

#include <sqee/debug/Logging.hpp>

string sq::get_time_string()
{
    auto now = std::time(nullptr);
    auto local = *std::localtime(&now);

    char buf[10];
    std::strftime(buf, 10, "%H:%M:%S", &local);

    return buf;
}
