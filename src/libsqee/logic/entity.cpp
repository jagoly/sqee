#include <iostream>

#include <logic/entity.hpp>
#include <logic/stage.hpp>

using namespace sq;

Entity::Entity() {
    name = "default";
    providesMap.emplace("debug");
}

std::string Entity::actionDebug() {
    std::cout << sizeof(this);
    return "Default Entity. Does nothing.";
}
