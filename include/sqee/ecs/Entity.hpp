#pragma once

#include <unordered_map>

#include <sqee/assert.hpp>
#include <sqee/builtins.hpp>
#include <sqee/ecs/Component.hpp>
#include <sqee/misc/Algorithms.hpp>

// Forward Declarations /////
namespace chaiscript { class Boxed_Value; }

namespace sq {

class EntityBase : sq::NonCopyable {
public:
    ~EntityBase();

    EntityBase(const string& _name);

    /// Create or get the specified attribute
    chai::Boxed_Value& operator[](const string& _key);

    /// Get the specified attribute
    chai::Boxed_Value& get_attr(const string& _key);

    /// Delete the specified attribute
    void del_attr(const string& _key);

    const string name;

private:
    unique_ptr<std::unordered_map<string, chai::Boxed_Value>> attributes;
};

template<class WorldType>
class Entity : public EntityBase {
public:
    using EntityBase::EntityBase;
    using ComponentType = Component<WorldType>;

    /// Returns the Entity's parent
    Entity* get_parent() { return parent; }

    /// Returns the Entity's parent (const)
    const Entity* get_parent() const { return parent; }

    /// Returns the Entity's children
    vector<unique_ptr<Entity>>& get_children() { return children; }

    /// Returns the Entity's children (const)
    const vector<unique_ptr<Entity>>& get_children() const { return children; }

    /// Add a new child to the Entity
    Entity* add_child(const string& _name) {
        SQASSERT(!try_get_child(_name), "entity already has child");
        children.emplace_back(new Entity<WorldType>(_name));
        children.back()->parent = this;
        return children.back().get();
    }

    /// Get the specified child
    Entity* get_child(const string& _name) {
        const Entity* constThis = const_cast<const Entity*>(this);
        return const_cast<Entity*>(constThis->get_child(_name));
    }

    /// Get the specified child (const)
    const Entity* get_child(const string& _name) const {
        auto predicate = [_name](const auto& child) { return child->name == _name; };
        auto iter = algo::find_if(children.begin(), children.end(), predicate);
        SQASSERT(iter != children.end(), "entity does not have child");
        return iter->get();
    }

    /// Try to get the specified child
    Entity* try_get_child(const string& _name) {
        const Entity* constThis = const_cast<const Entity*>(this);
        return const_cast<Entity*>(constThis->try_get_child(_name));
    }

    /// Try to get the specified child (const)
    const Entity* try_get_child(const string& _name) const {
        auto predicate = [_name](const auto& child) { return child->name == _name; };
        auto iter = algo::find_if(children.begin(), children.end(), predicate);
        if (iter == children.end()) return nullptr;
        return iter->get();
    }

    /// Remove a child from the Entity
    void remove_child(const string& _name, WorldType& _world) {
        auto predicate = [_name](const auto& child) { return child->name == _name; };
        auto iter = algo::find_if(children.begin(), children.end(), predicate);
        SQASSERT(iter != children.end(), "entity does not have child");
        _world.clean_up_entity(iter->get());
        children.erase(iter);
    }

    /// Add a component to the Entity
    template<class T> T* add_component(WorldType& _world) {
        static_assert(std::is_base_of<ComponentType, T>::value, "");
        SQASSERT(!try_get_component<T>(), "entity already has component");
        components.emplace_back(type_index(typeid(T)), nullptr);
        components.back().second.reset(new T(*this, _world));
        return static_cast<T*>(components.back().second.get());
    }

    /// Get the specified component
    template<class T> T* get_component() {
        const Entity* constThis = const_cast<const Entity*>(this);
        return const_cast<T*>(constThis->get_component<T>());
    }

    /// Get the specified component (const)
    template<class T> const T* get_component() const {
        static_assert(std::is_base_of<ComponentType, T>::value, "");
        auto iter = algo::find_first(components, type_index(typeid(T)));
        SQASSERT(iter != components.end(), "missing component");
        return static_cast<T*>(iter->second.get());
    }

    /// Try to get the specified component
    template<class T> T* try_get_component() {
        const Entity* constThis = const_cast<const Entity*>(this);
        return const_cast<T*>(constThis->try_get_component<T>());
    }

    /// Try to get the specified component (const)
    template<class T> const T* try_get_component() const {
        static_assert(std::is_base_of<ComponentType, T>::value, "");
        auto iter = algo::find_first(components, type_index(typeid(T)));
        if (iter == components.end()) return nullptr;
        return static_cast<T*>(iter->second.get());
    }

    /// Remove a component from the Entity
    template<class T> void remove_component(WorldType& _world) {
        static_assert(std::is_base_of<ComponentType, T>::value, "");
        auto iter = algo::find_first(components, type_index(typeid(T)));
        SQASSERT(iter != components.end(), "missing component");
        T* ptr = static_cast<T*>(iter->second.get());
        _world.clean_up_component(ptr, this);
        components.erase(iter);
    }

    /// Run one or more functions for entities with the specified component
    template<class T, class... Funcs> void propogate_forward(Funcs&&... _funcs) {
        if (auto* c = try_get_component<T>()) expand{(std::forward<Funcs>(_funcs)(c, this), 0)...};
        for (auto& c : children) c->propogate_forward<T, Funcs...>(std::forward<Funcs>(_funcs)...);
    }

    /// Run one or more functions for entities with the specified component
    template<class T, class... Funcs> void propogate_reverse(Funcs&&... _funcs) {
        for (auto& c : children) c->propogate_forward<T, Funcs...>(std::forward<Funcs>(_funcs)...);
        if (auto* c = try_get_component<T>()) expand{(std::forward<Funcs>(_funcs)(c, this), 0)...};
    }

private:
    Entity* parent = nullptr; vector<unique_ptr<Entity>> children;
    vector<pair<type_index, unique_ptr<ComponentType>>> components;
};

}
