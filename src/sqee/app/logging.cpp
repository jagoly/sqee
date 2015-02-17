#include <cstdarg>
#include <ctime>
#include <iostream>
#include <memory>

#include "app/logging.hpp"

using namespace sq;

const string get_time() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[10];
    tstruct = *std::localtime(&now);
    std::strftime(buf, sizeof(buf), "%H:%M:%S ", &tstruct);

    return buf;
}

void sq::log_only(const string& _message, string _a0, string _a1) {
    string msgStr = _message; size_t pos;

    pos = msgStr.find("$0", 0);
    if (pos != string::npos) msgStr.replace(pos, 2, '\"'+_a0+'\"');

    pos = msgStr.find("$1", 0);
    if (pos != string::npos) msgStr.replace(pos, 2, _a1);

    std::cout << get_time() << msgStr << std::endl;
}

void sq::log_info(const string& _message, string _a0, string _a1) {
    log_only("INFO: "+_message, _a0, _a1);
}

void sq::log_warning(const string& _message, string _a0, string _a1) {
    log_only("WARNING: "+_message, _a0, _a1);
}

void sq::log_error(const string& _message, string _a0, string _a1) {
    log_only("ERROR: "+_message, _a0, _a1);
}
