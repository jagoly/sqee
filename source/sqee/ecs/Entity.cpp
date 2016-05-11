#include <chaiscript/dispatchkit/boxed_value.hpp>

#include <sqee/ecs/Entity.hpp>

using namespace sq;

Entity::~Entity() = default;

Entity::Entity(const string& _name) : name(_name),
    attributes(new std::unordered_map<string, chai::Boxed_Value>()) {
}

Entity* Entity::add_child(const string& _name) {
    auto newChild = new Entity(_name);
    children.emplace_back(newChild);
    newChild->set_parent(this);
    return newChild;
}

void Entity::set_attr(const string& _name, chai::Boxed_Value& _value) {
    attributes->operator[](_name) = _value;
}

chai::Boxed_Value& Entity::get_attr(const string& _name) {
    return attributes->at(_name);
}

void Entity::erase_attr(const string& _name) {
    attributes->erase(_name);
}

void Entity::clear_marked_refresh() {
    for (auto& c : components) c.second->needsRefresh = false;
    for (auto& child : children) child->clear_marked_refresh();
}
