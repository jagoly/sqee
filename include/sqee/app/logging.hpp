#pragma once
#include "forward.hpp"

namespace sq {

void log_only    (const string& _message, string _a0 = "", string _a1 = "");
void log_info    (const string& _message, string _a0 = "", string _a1 = "");
void log_warning (const string& _message, string _a0 = "", string _a1 = "");
void log_error   (const string& _message, string _a0 = "", string _a1 = "");

}
