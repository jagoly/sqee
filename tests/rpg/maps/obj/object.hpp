#pragma once

namespace sqt {

enum class ObjType {
    Model, Reflector, Liquid, Light
};

struct ObjectSpec {
    ObjectSpec(const string& _name, ObjType _type, glm::vec3 _cellPos)
        : name(_name), type(_type), cellPos(_cellPos) {}
    const string name;
    const ObjType type;
    const glm::vec3 cellPos;

    void parse_line(const vector<string>& _line);

    map<string, vector<bool>> bMap;
    map<string, vector<int>> iMap;
    map<string, vector<float>> fMap;
    map<string, vector<string>> sMap;
    set<string> flagSet;
};

class Object {
public:
    Object(const ObjectSpec& _spec)
        : name(_spec.name), type(_spec.type), cellPos(_spec.cellPos) {}

    const string name;
    const ObjType type;
    const glm::vec3 cellPos;

    virtual void tick() {}
    virtual void calc(double _accum) {}
};

}

#include "model.hpp"
#include "liquid.hpp"
#include "reflector.hpp"
#include "light.hpp"
