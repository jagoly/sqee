#include <iostream>

#include <libsqee/logic/entity.hpp>

using namespace sq;

Entity::Entity() {
    name = "Default Entity";
}

void Entity::add_component(std::string strId, Component* comp) {
    componentMap.insert(std::pair<std::string, std::unique_ptr<Component>>
                        (strId, std::unique_ptr<Component>(std::move(comp))));
}

void Entity::remove_component(std::string strId) {
    if (componentMap.count(strId)) {
        componentMap.erase(strId);
        return;
    }
    throw;
}

Component& Entity::get_component_for_action(std::string actionStr) {
    for (auto& comp : componentMap) {
        if (comp.second->actionStringSet.count(actionStr)) {
            return *comp.second;
        }
    }
    throw;
}
