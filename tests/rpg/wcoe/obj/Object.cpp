#include <sqee/misc/Files.hpp>

#include "Object.hpp"

using namespace sqt::wcoe;

void ObjSpec::parse_line(const vector<string>& _line) {
    const string& key = _line[0];

    if (key == "flags") {
        for (uint i = 1u; i < _line.size(); i++)
            flagSet.emplace(_line[i]);
    }

    else if (key == "int") intMap.emplace(_line[1], stoi(_line[2]));
    else if (key == "uint") uintMap.emplace(_line[1], stou(_line[2]));
    else if (key == "float") floatMap.emplace(_line[1], stof(_line[2]));
    else if (key == "bool") boolMap.emplace(_line[1], _line[2] == "true");
    else if (key == "string") stringMap.emplace(_line[1], _line[2]);
    else if (key == "fquat") fquatMap.emplace(_line[1], svtofeu(_line, 2));
    else if (key == "ivec2") ivec2Map.emplace(_line[1], svtoiv2(_line, 2));
    else if (key == "ivec3") ivec3Map.emplace(_line[1], svtoiv3(_line, 2));
    else if (key == "ivec4") ivec4Map.emplace(_line[1], svtoiv4(_line, 2));
    else if (key == "uvec2") uvec2Map.emplace(_line[1], svtouv2(_line, 2));
    else if (key == "uvec3") uvec3Map.emplace(_line[1], svtouv3(_line, 2));
    else if (key == "uvec4") uvec4Map.emplace(_line[1], svtouv4(_line, 2));
    else if (key == "fvec2") fvec2Map.emplace(_line[1], svtofv2(_line, 2));
    else if (key == "fvec3") fvec3Map.emplace(_line[1], svtofv3(_line, 2));
    else if (key == "fvec4") fvec4Map.emplace(_line[1], svtofv4(_line, 2));
    else throw std::runtime_error("invalid type \"" + key + "\"");
}


SpecException::SpecException(const string& _key, const string& _type, const string& _object)
    : key(_key), type(_type), object(_object) {}

const char* SpecException::what() noexcept {
    message = "missing key \"" + key + "\" of type \""
            + type + "\" in object \"" + object + "\"";
    return message.c_str();
}


Object::Object(ObjType _type, const string& _name, Cell* _cell)
    : type(_type), name(_name), cell(_cell) {}
