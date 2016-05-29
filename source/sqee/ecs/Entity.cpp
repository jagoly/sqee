#include <chaiscript/dispatchkit/boxed_value.hpp>

#include <sqee/ecs/Entity.hpp>

using namespace sq;

EntityBase::~EntityBase() = default;

EntityBase::EntityBase(const string& _name) : name(_name),
    attributes(new std::unordered_map<string, chai::Boxed_Value>()) {
}

chai::Boxed_Value& EntityBase::operator[](const string& _key) {
    return attributes->operator[](_key);
}

chai::Boxed_Value& EntityBase::get_attr(const string& _key) {
    return attributes->at(_key);
}

void EntityBase::del_attr(const string& _key) {
    attributes->erase(_key);
}
