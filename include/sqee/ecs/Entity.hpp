#pragma once

#include <unordered_map>

#include <sqee/assert.hpp>
#include <sqee/builtins.hpp>
#include <sqee/ecs/Component.hpp>
#include <sqee/misc/Algorithms.hpp>

// Forward Declarations /////
namespace chaiscript { class Boxed_Value; }

namespace sq {

class Entity : NonCopyable {
public:
    Entity(Entity* _parent);

    ~Entity();

    void mark_dirty() { dirtyFlag = true; }
    bool check_dirty() const { return dirtyFlag; }


    /// Create or get the specified attribute
    chai::Boxed_Value& operator[](const string& _key);

    /// Get the specified attribute
    chai::Boxed_Value& get_attr(const string& _key);

    /// Delete the specified attribute
    void del_attr(const string& _key);


    /// Returns the Entity's parent
    Entity* get_parent() { return parent; }

    /// Returns the Entity's parent (const)
    const Entity* get_parent() const { return parent; }

    /// Returns the Entity's children
    vector<unique_ptr<Entity>>& get_children() { return children; }

    /// Returns the Entity's children (const)
    const vector<unique_ptr<Entity>>& get_children() const { return children; }


    /// Add a component to the Entity
    template<class T> T* enable_component() {
        static_assert(std::is_base_of<Component, T>::value, "");
        SQASSERT(!try_get_component<T>(), "entity already has component");
        components.emplace_back(type_index(typeid(T)), nullptr);
        components.back().second.reset(new T(*this));
        return static_cast<T*>(components.back().second.get());
    }   

    /// Remove a component from the Entity
    template<class T> void disable_component() {
        static_assert(std::is_base_of<Component, T>::value, "");
        auto iter = algo::find_first(components, type_index(typeid(T)));
        SQASSERT(iter != components.end(), "missing component");
        components.erase(iter);
    }


    /// Get the specified component
    template<class T> T* get_component() {
        const Entity* constThis = const_cast<const Entity*>(this);
        return const_cast<T*>(constThis->get_component<T>());
    }

    /// Get the specified component (const)
    template<class T> const T* get_component() const {
        static_assert(std::is_base_of<Component, T>::value, "");
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
        static_assert(std::is_base_of<Component, T>::value, "");
        auto iter = algo::find_first(components, type_index(typeid(T)));
        if (iter == components.end()) return nullptr;
        return static_cast<T*>(iter->second.get());
    }


    /// Propogate a function to all descendents
    template<class Func> void propogate(Func&& _func) {
        _func(this); for (auto& c : children) c->propogate(_func);
    }

private:
    Entity* parent; string uniqueName = "";

    bool dirtyFlag = true;

    unique_ptr<std::unordered_map<string, chai::Boxed_Value>> attributes;
    vector<pair<type_index, unique_ptr<Component>>> components;
    vector<unique_ptr<Entity>> children;

    friend class EntityManager;
};

}
