#include <sqee/misc/Files.hpp>

#include "Object.hpp"

using namespace sqt::wcoe;

void ObjSpec::parse_line(const vector<string>& _line) {
    const string& key = _line[0];

    if (key == "flags") {
        for (uint i = 1; i < _line.size(); i++)
            flags.emplace(_line[i]);
        return;
    }

    char c = key[0];
    if (c == 'i' || c == 'b' || c == 'f' || c == 's') {
        int len = stoi(key.substr(1, 1));
        const string& name = _line[1];

        if (c == 'i') {
            vector<int>& vec = iMap[name];
            for (int i = 0; i < len; i++)
                vec.push_back(stoi(_line[i+2]));
        } else
        if (c == 'b') {
            vector<bool>& vec = bMap[name];
            for (int i = 0; i < len; i++)
                vec.push_back(stoi(_line[i+2]));
        } else
        if (c == 'f') {
            vector<float>& vec = fMap[name];
            for (int i = 0; i < len; i++)
                vec.push_back(stof(_line[i+2]));
        } else
        if (c == 's') {
            vector<string>& vec = sMap[name];
            for (int i = 0; i < len; i++)
                vec.push_back(_line[i+2]);
        }
        return;
    }
}


Object::Object(ObjType _type, const string& _name, Cell* _cell)
    : type(_type), name(_name), cell(_cell) {}
