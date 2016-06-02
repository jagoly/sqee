#include <ctime>

#include <sqee/debug/Logging.hpp>

string sq::get_time_string() {
    std::time_t now = std::time(nullptr);
    std::tm timeStruct; char buf[10];
    timeStruct = *std::localtime(&now);
    std::strftime(buf, sizeof(buf), "%H:%M:%S", &timeStruct);
    return buf;
}
