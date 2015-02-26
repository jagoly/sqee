#pragma once
#include <sqee/forward.hpp>

#include <map>
#include <set>
#include <vector>

namespace sqt {

enum class ObjType {
    Model, Reflector, Liquid, Light
};

struct ObjectSpec {
    ObjectSpec(const string& _name, ObjType _type, glm::vec3 _cellPos);
    const string name;
    const ObjType type;
    const glm::vec3 cellPos;

    void parse_line(const std::vector<string>& _line);

    std::map<string, std::vector<bool>> bMap;
    std::map<string, std::vector<int>> iMap;
    std::map<string, std::vector<float>> fMap;
    std::map<string, std::vector<string>> sMap;
    std::set<string> flagSet;
};

class Object {
public:
    Object(const ObjectSpec& _spec);

    const string name;
    const ObjType type;
    const glm::vec3 cellPos;

    virtual void tick() {}
    virtual void calc(double _accum) {}
};

}
