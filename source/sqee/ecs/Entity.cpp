#include <chaiscript/dispatchkit/boxed_value.hpp>

#include <sqee/ecs/Entity.hpp>

using namespace sq;


Entity::Entity( sq::Entity* _parent) : parent(_parent),
    attributes(new std::unordered_map<string, chai::Boxed_Value>()) {
}

Entity::~Entity() {
}

chai::Boxed_Value& Entity::operator[](const string& _key) {
    return attributes->operator[](_key);
}

chai::Boxed_Value& Entity::get_attr(const string& _key) {
    return attributes->at(_key);
}

void Entity::del_attr(const string& _key) {
    attributes->erase(_key);
}
