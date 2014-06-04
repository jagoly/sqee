#pragma once

#include <string>
#include <set>
#include <memory>

namespace sq {

class Stage;

class Entity {
public:
    Entity();
    Stage* stage;

    std::string name;
    std::set<std::string> providesMap;

    std::string actionDebug();
};

}
