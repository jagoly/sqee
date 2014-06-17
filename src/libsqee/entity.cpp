#include <iostream>

#include <entity.hpp>

namespace sq {

Entity::Entity() {
    name = "default";
    providesMap.emplace("debug");
}

std::string Entity::actionDebug() {
    std::cout << sizeof(this);
    return "Default Entity. Does nothing.";
}

}
