#include <ctime>

#include <sqee/app/logging.hpp>

string sq::get_time_str() {
    std::time_t now = time(0);
    struct tm tstruct;
    char buf[10];
    tstruct = *std::localtime(&now);
    std::strftime(buf, sizeof(buf), "%H:%M:%S", &tstruct);
    return buf;
}
