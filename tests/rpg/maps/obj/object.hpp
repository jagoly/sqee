#pragma once

#include "../../resbank.hpp"

namespace sqt {
namespace wld {

enum class ObjType {
    Model, Liquid, Data, Light
};

struct ObjectSpec {
    void parse_line(const string& _line);

    ObjType objType;
    string uid;

    map<string, vector<bool>> boolMap;
    map<string, vector<int>> intMap;
    map<string, vector<float>> floatMap;
    map<string, vector<string>> stringMap;
    set<string> flagSet;

    glm::ivec2 xyOffs;
    float zOffs;
};

class Object {
public:
    Object(ObjType _type, const string& _uid)
        : type(_type), uid(_uid) {}

    virtual ~Object() {}

    virtual void create(ObjectSpec& _spec) {}

    ObjType type;
    string uid;

    virtual void tick() {}
    virtual void calc(double _accum) {}
};

}
}

#include "model.hpp"
#include "liquid.hpp"
#include "data.hpp"
#include "light.hpp"
