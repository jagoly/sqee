#pragma once

#include <unordered_map>
#include <unordered_set>

#include <chaiscript/dispatchkit/boxed_cast.hpp>
#include <chaiscript/dispatchkit/boxed_value.hpp>

#include <sqee/assert.hpp>
#include <sqee/forward.hpp>
#include <sqee/builtins.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Quaternion.hpp>
#include <sqee/maths/Volumes.hpp>

// Forward Declarations /////
namespace chaiscript { class Boxed_Value; }

namespace sqt { namespace wcoe {

class Entity;

class Component : sq::NonCopyable {
public:
    virtual ~Component() = default;

    virtual void setup_depends(Entity*) {}

    void mark_needsUpdate() {
        if (needsUpdate == false) { needsUpdate = true;
            for (auto rDep : rDepends) rDep->mark_needsUpdate();
        }
    }

    vector<Component*> rDepends;
    bool needsUpdate = true;
};


class Entity : sq::NonCopyable {
public:
    Entity(const string& _name) : name(_name) {};

    void set_parent(Entity* _parent) {
        parent = _parent;
    }

    Entity* get_parent() {
        return parent;
    }

    const Entity* get_parent() const {
        return parent;
    }

    Entity* add_child(const string& _name) {
        auto newChild = new Entity(_name);
        children.emplace_back(newChild);
        newChild->set_parent(this);
        return newChild;
    }

    vector<Entity*> get_children() {
        vector<Entity*> vecRaw; vecRaw.reserve(children.size());
        for (auto& child : children) vecRaw.push_back(child.get());
        return vecRaw;
    }

    vector<const Entity*> get_children() const {
        vector<const Entity*> vecRaw; vecRaw.reserve(children.size());
        for (const auto& child : children) vecRaw.push_back(child.get());
        return vecRaw;
    }

    template<class ComponentType> ComponentType* add_component() {
        static_assert(std::is_base_of<Component, ComponentType>::value, "");
        SQASSERT(!get_component(ComponentType::type()), "entity already has component");
        auto newComponent = new ComponentType(); newComponent->setup_depends(this);
        components.emplace(ComponentType::type(), unique_ptr<Component>(newComponent));
        return newComponent;
    }

    Component* get_component(const string& _type) {
        const auto iter = components.find(_type);
        if (iter == components.end()) return nullptr;
        return iter->second.get();
    }

    const Component* get_component(const string& _type) const {
        const auto iter = components.find(_type);
        if (iter == components.end()) return nullptr;
        return iter->second.get();
    }

    template<class T> T* get_component(const string& _type) {
        static_assert(std::is_base_of<Component, T>::value, "");
        return static_cast<T*>(get_component(_type));
    }

    template<class T> const T* get_component(const string& _type) const {
        static_assert(std::is_base_of<Component, T>::value, "");
        return static_cast<const T*>(get_component(_type));
    }

    vector<Component*> get_components() {
        vector<Component*> vecRaw; vecRaw.reserve(components.size());
        for (auto& c : components) vecRaw.push_back(c.second.get());
        return vecRaw;
    }

    vector<const Component*> get_components() const {
        vector<const Component*> vecRaw; vecRaw.reserve(components.size());
        for (const auto& c : components) vecRaw.push_back(c.second.get());
        return vecRaw;
    }

    void clear_needsUpdate() {
        for (auto& c : components) c.second->needsUpdate = false;
        for (auto& child : children) child->clear_needsUpdate();
    }

//    template<class T> T& get_attribute(const string& _name) {
//        return chai::boxed_cast<T&>(attributes.at(_name));
//    }

//    chai::Boxed_Value& get_attribute(const string& _name) {
//        return attributes.at(_name);
//    }

//    template<class T> void set_attribute(const string& _name, T _value) {
//        attributes.emplace(_name, _value);
//    }

//    void set_attribute(const string& _name, chai::Boxed_Value _value) {
//        attributes.emplace(_name, _value);
//    }

    const string name;

private:
    Entity* parent = nullptr;
    vector<unique_ptr<Entity>> children;
    //vector<unique_ptr<Component>> components;
    std::unordered_map<string, unique_ptr<Component>> components;
//    std::unordered_map<string, chai::Boxed_Value> attributes;
};


class TransformComponent : public Component {
public:
    static string type() { return "Transform"; }

    void setup_depends(Entity* _e) {
        if (Entity* parent = _e->get_parent()) {
            if (auto pcTransform = parent->get_component<TransformComponent>("Transform"))
                pcTransform->rDepends.push_back(this), PRNT_transform = pcTransform;
        }
    }

    // User Properties /////
    Vec3F PROP_position = {0.f, 0.f, 0.f};
    QuatF PROP_rotation = {0.f, 0.f, 0.f, 1.f};
    Vec3F PROP_scale    = {1.f, 1.f, 1.f};

    // System Properties /////
    bool negScale;
    Mat4F matrix;

    // Soft Depended Components /////
    const TransformComponent* PRNT_transform = nullptr;
};


class ModelComponent : public Component {
public:
    static string type() { return "Model"; }

    ModelComponent() { ubo.reserve("matrix", 16u),
                       ubo.reserve("normMat", 16u),
                       ubo.allocate_storage(); }

    void setup_depends(Entity* _e) {
        auto cTransform = _e->get_component<TransformComponent>("Transform");
        SQASSERT(cTransform != nullptr, "'Model' is dependant on 'Transform'");
        cTransform->rDepends.push_back(this), DEP_transform = cTransform;
    }

    // User Properties /////
    bool PROP_render = true;
    bool PROP_shadow = true;
    bool PROP_decals = true;
    string PROP_mesh = "";
    string PROP_skin = "";

    // System Properties /////
    sq::UniformBuffer ubo;
    sq::BoundBox bbox;
    sq::Mesh* mesh;
    sq::Skin* skin;

    // Hard Depended Components /////
    const TransformComponent* DEP_transform;
};

}}
