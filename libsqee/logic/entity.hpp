#ifndef ENTITY_HPP
#define ENTITY_HPP

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

#endif // ENTITY_HPP
