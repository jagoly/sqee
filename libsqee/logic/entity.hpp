#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>
#include <map>
#include <memory>

#include <libsqee/logic/component.hpp>

namespace sq {

class Application;

class Entity {
public:
    Entity();
    Application* application;

    std::string name;

    void add_component(std::string, Component*);
    void remove_component(std::string);
    Component& get_component_for_action(std::string);
protected:
    std::map<std::string, std::unique_ptr<Component>> componentMap;
};

}

#endif // ENTITY_HPP
