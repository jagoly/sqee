#include "settings.hpp"

using namespace sqt;

Settings::Settings() {
    intMap["shadows"] = {4, 4};
    intMap["antialiasing"] = {2, 2};
}

int Settings::get_int(const string& _setting) {
    return intMap.at(_setting).first;
}

float Settings::get_float(const string& _setting) {
    return floatMap.at(_setting).first;
}

string Settings::get_string(const string& _setting) {
    return stringMap.at(_setting).first;
}
