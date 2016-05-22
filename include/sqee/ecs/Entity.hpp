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

    /// Returns the Entity's parent
    Entity* get_parent() { return parent; }

    /// Returns the Entity's parent (const)
    const Entity* get_parent() const { return parent; }

    /// Add a new child to the Entity
    Entity* add_child(const string& _name);

    /// Get the specified child or nullptr
    Entity* get_child(const string& _name);

    /// Get the specified child or nullptr (const)
    const Entity* get_child(const string& _name) const;

    /// Remove a child from the Entity
    void remove_child(const string& _name);

    /// Returns the Entity's children
    vector<unique_ptr<Entity>>& get_children() { return children; }

    /// Returns the Entity's children (const)
    const vector<unique_ptr<Entity>>& get_children() const { return children; }

    /// Add a component to the Entity
    template<class T> T* add_component() {
        static_assert(std::is_base_of<Component, T>::value, "");
        SQASSERT(!get_component<T>(), "entity already has component");
        unique_ptr<Component>& uptr = components[type_index(typeid(T))];
        uptr.reset(new T()); return static_cast<T*>(uptr.get());
    }

    /// Get the specified component or nullptr
    template<class T> T* get_component() {
        static_assert(std::is_base_of<Component, T>::value, "");
        auto iter = components.find(type_index(typeid(T)));
        if (iter == components.end()) return nullptr;
        return static_cast<T*>(iter->second.get());
    }

    /// Get the specified component or nullptr (const)
    template<class T> const T* get_component() const {
        static_assert(std::is_base_of<Component, T>::value, "");
        auto iter = components.find(type_index(typeid(T)));
        if (iter == components.end()) return nullptr;
        return static_cast<T*>(iter->second.get());
    }

    /// Remove a component from the Entity
    template<class T> void remove_component() {
        SQASSERT(get_component<T>(), "entity does not have component");
        components.erase(type_index(typeid(T)));
    }

    /// Mark the specified component for configure
    template<class T/*, ecs::if_Configure<T>...*/>
    void mark_configure_component(bool _recursive) {
        if (auto c = get_component<T>()) c->needsConfigure = true;
        if (_recursive == true) for (auto& child : children)
                child->mark_configure_component<T>(true);
    }

    /// Mark the specified component for refresh
    template<class T> void mark_refresh_component(bool _recursive) {
        if (auto c = get_component<T>()) c->needsRefresh = true;
        if (_recursive == true) for (auto& child : children)
                child->mark_refresh_component<T>(true);
    }

    /// Mark all components for configure
    void mark_configure_all(bool _recursive);

    /// Mark all components for refresh
    void mark_refresh_all(bool _recursive);

    /// Create or get the specified attribute
    chai::Boxed_Value& operator[](const string& _key);

    /// Get the specified attribute
    chai::Boxed_Value& get_attr(const string& _key);

    /// Erase the specified attribute
    void erase_attr(const string& _key);

    const string name;

private:
    Entity* parent = nullptr; vector<unique_ptr<Entity>> children;
    std::map<const std::type_index, unique_ptr<Component>> components;
    unique_ptr<std::unordered_map<string, chai::Boxed_Value>> attributes;
};

}
