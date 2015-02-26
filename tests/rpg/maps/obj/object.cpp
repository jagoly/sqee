#include <sqee/gl/gl_ext_3_3.hpp>
#include <sqee/misc/files.hpp>

#include "object.hpp"

using namespace sqt;

ObjectSpec::ObjectSpec(const string& _name, ObjType _type, glm::vec3 _cellPos)
    : name(_name), type(_type), cellPos(_cellPos)
{}


Object::Object(const ObjectSpec& _spec)
    : name(_spec.name), type(_spec.type), cellPos(_spec.cellPos)
{}

void ObjectSpec::parse_line(const std::vector<string>& _line) {
    const string& key = _line[0];

    if (key == "flags") {
        for (uint i = 1; i < _line.size(); i++)
            flagSet.emplace(_line[i]);
        return;
    }

    char c = key[0];
    if (c == 'i' || c == 'f' || c == 's') {
        int len = std::atoi(key.substr(1, 1).c_str());
        const string& name = _line[1];

        if (c == 'i') {
            std::vector<int>& vec = iMap[name];
            for (int i = 0; i < len; i++)
                vec.push_back(std::stoi(_line[i+2]));
        } else
        if (c == 'f') {
            std::vector<float>& vec = fMap[name];
            for (int i = 0; i < len; i++)
                vec.push_back(std::stof(_line[i+2]));
        } else
        if (c == 's') {
            std::vector<string>& vec = sMap[name];
            for (int i = 0; i < len; i++)
                vec.push_back(_line[i+2]);
        }
        return;
    }
}
