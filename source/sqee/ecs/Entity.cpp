#include <algorithm>

#include <chaiscript/dispatchkit/boxed_value.hpp>

#include <sqee/ecs/Entity.hpp>

using namespace sq;

Entity::~Entity() = default;

Entity::Entity(const string& _name) : name(_name),
    attributes(new std::unordered_map<string, chai::Boxed_Value>()) {
}

Entity* Entity::add_child(const string& _name) {
    SQASSERT(!get_child(_name), "entity alread has child");
    children.emplace_back(new Entity(_name));
    children.back()->parent = this;
    return children.back().get();
}

Entity* Entity::get_child(const string& _name) {
    auto predicate = [_name](const auto& child) { return child->name == _name; };
    auto iter = std::find_if(children.begin(), children.end(), predicate);
    if (iter == children.end()) return nullptr; return iter->get();
}

const Entity* Entity::get_child(const string& _name) const {
    auto predicate = [_name](const auto& child) { return child->name == _name; };
    auto iter = std::find_if(children.begin(), children.end(), predicate);
    if (iter == children.end()) return nullptr; return iter->get();
}

void Entity::remove_child(const string& _name) {
    auto predicate = [_name](auto& child) { return child->name == _name; };
    auto iter = std::find_if(children.begin(), children.end(), predicate);
    SQASSERT(iter != children.end(), "entity does not have child");
    children.erase(iter);
}

void Entity::mark_configure_all(bool _recursive) {
    for (auto& c : components)
        if (auto p = dynamic_cast<ecs::TagConfigure*>(c.second.get()))
            p->needsConfigure = true;
    if (_recursive == true)
        for (auto& child : children)
            child->mark_configure_all(true);
}

void Entity::mark_refresh_all(bool _recursive) {
    for (auto& c : components)
        if (auto p = dynamic_cast<ecs::TagRefresh*>(c.second.get()))
            p->needsRefresh = true;
    if (_recursive == true)
        for (auto& child : children)
            child->mark_refresh_all(true);
}

chai::Boxed_Value& Entity::operator[](const string& _key) {
    return attributes->operator[](_key);
}

chai::Boxed_Value& Entity::get_attr(const string& _key) {
    return attributes->at(_key);
}

void Entity::erase_attr(const string& _key) {
    attributes->erase(_key);
}
