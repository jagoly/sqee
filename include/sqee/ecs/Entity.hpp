#pragma once

#include <map>
#include <unordered_map>

#include <sqee/assert.hpp>
#include <sqee/builtins.hpp>
#include <sqee/ecs/Component.hpp>

// Forward Declarations /////
namespace chaiscript { class Boxed_Value; }

namespace sq {

class Entity : sq::NonCopyable {
public:
     ~Entity();

    Entity(const string& _name);

    Entity* get_parent() { return parent; }

    const Entity* get_parent() const { return parent; }

    vector<unique_ptr<Entity>>& get_children() { return children; }

    const vector<unique_ptr<Entity>>& get_children() const { return children; }

    void set_parent(Entity* _parent) { parent = _parent; }

    Entity* add_child(const string& _name);

    template<class T> T* add_component() {
        static_assert(std::is_base_of<Component, T>::value, "");
        SQASSERT(!get_component<T>(), "entity already has component");
        auto newComponent = new T(); newComponent->setup_depends(this);
        components[type_index(typeid(T))].reset(newComponent);
        return newComponent;
    }

    template<class T> T* get_component() {
        static_assert(std::is_base_of<Component, T>::value, "");
        const auto iter = components.find(type_index(typeid(T)));
        if (iter == components.end()) return nullptr;
        return static_cast<T*>(iter->second.get());
    }

    template<class T> const T* get_component() const {
        static_assert(std::is_base_of<Component, T>::value, "");
        const auto iter = components.find(type_index(typeid(T)));
        if (iter == components.end()) return nullptr;
        return static_cast<T*>(iter->second.get());
    }

    void set_attr(const string& _name, chai::Boxed_Value& _value);

    chai::Boxed_Value& get_attr(const string& _name);

    void erase_attr(const string& _name);

    void clear_marked_refresh();

    const string name;

private:
    Entity* parent = nullptr; vector<unique_ptr<Entity>> children;
    std::map<const std::type_index, unique_ptr<Component>> components;
    unique_ptr<std::unordered_map<string, chai::Boxed_Value>> attributes;
};

}
