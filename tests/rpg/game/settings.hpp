#pragma once

#include <sqee/defs.hpp>

namespace sqt {

class Settings {
public:
    Settings();

    int get_int(const string& _setting);
    int get_int_next(const string& _setting);
    float get_float(const string& _setting);
    float get_float_next(const string& _setting);
    string get_string(const string& _setting);
    string get_string_next(const string& _setting);

    void set_next(const string& _setting);

    void apply();
    void revert();
    void reset();

private:
    map<const string, pair<int, int>> intMap;
    map<const string, pair<float, float>> floatMap;
    map<const string, pair<string, string>> stringMap;
};

}
