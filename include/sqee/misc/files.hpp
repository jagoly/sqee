#pragma once

#include <redist/jsoncpp.hpp>

struct SubStr {
    SubStr(const std::string& _str) {
        a = _str.find_first_not_of(' ');
        b = _str.find_first_of(' ', a) - a;
        str = _str.substr(a, b);
    }
    SubStr(const std::string& _str, const SubStr& _prev) {
        a   = _str.find_first_not_of(' ', _prev.a + _prev.b);
        b   = _str.find_first_of(' ', a) - a;
        if (a <= _str.size())
            str = _str.substr(a, b);
    }
    uint a=0, b=0; std::string str="";
    operator float() const { return std::atof(str.c_str()); }
    explicit operator int() const { return std::atoi(str.c_str()); }
    explicit operator uint() const { return int(); }
};

namespace sq {

Json::Value get_json_from_file(const std::string& _path);

std::string get_string_from_file(const std::string& _path);

}
