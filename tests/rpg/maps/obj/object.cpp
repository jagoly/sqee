#include <iostream>

#include <sqee/misc/files.hpp>

#include "object.hpp"
#include "model.hpp"
#include "liquid.hpp"
#include "data.hpp"
#include "light.hpp"

using namespace sqt::wld;

void ObjectSpec::parse_line(const string& line) {
    SubStr key(line);

    if (key.str == "object") {
        SubStr val(line, key);
        if      (val.str == "model")   objType = ObjType::Model;
        else if (val.str == "liquid") objType = ObjType::Liquid;
        else throw; // bad type

        uid = SubStr(line, val).str;
        return;
    }

    if (key.str == "flags") {
        SubStr val = key;
        while (true) {
            val = SubStr(line, val);
            if (val.str.empty()) break;
            flagSet.emplace(val.str);
        }
        return;
    }

    char c = key.str[0];
    if (c == 'b' || c == 'i' || c == 'f' || c == 's') {
        int len = std::atoi(&key.str[1]);
        key = SubStr(line, key);
        SubStr val = key;

        if (c == 'b') {
            std::vector<bool>& vec = boolMap[key.str];
            for (int i = 0; i < len; i++) {
                val = SubStr(line, val);
                vec.push_back(int(val));
            }
        }
        if (c == 'i') {
            std::vector<int>& vec = intMap[key.str];
            for (int i = 0; i < len; i++) {
                val = SubStr(line, val);
                vec.push_back(int(val));
            }
        }
        if (c == 'f') {
            std::vector<float>& vec = floatMap[key.str];
            for (int i = 0; i < len; i++) {
                val = SubStr(line, val);
                vec.push_back(float(val));
            }
        }
        if (c == 's') {
            std::vector<string>& vec = stringMap[key.str];
            for (int i = 0; i < len; i++) {
                val = SubStr(line, val);
                vec.push_back(val.str);
            }
        }
        return;
    }
}
