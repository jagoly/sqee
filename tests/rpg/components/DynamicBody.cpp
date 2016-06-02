#include <rp3d/engine/DynamicsWorld.hpp>

#include "../wcoe/Camera.hpp"
#include "../wcoe/World.hpp"
#include "DynamicBody.hpp"
#include "Transform.hpp"

namespace sqt {
namespace maths = sq::maths;

DynamicBodyComponent::DynamicBodyComponent(EntityRPG& _e, World& _w) : body(_w.get_PhysicsWorld()) {
    const auto* transform = _e.get_component<TransformComponent>();
    body.set_transform(transform->PROP_position, transform->PROP_rotation);

    if (_e.name == "DicePhys") {
        body.add_BoxShape(0u, Vec3F(0.25f), 30.f);
        body.set_bounciness(0.3f); body.set_friction(0.3f);
        body.set_linearDamp(0.2f); body.set_angularDamp(0.2f);
    }

    if (_e.name == "BallPhys") {
        body.add_SphereShape(0u, 0.25f, 5.f);
        body.set_bounciness(0.7f); body.set_friction(0.6f);
        body.set_linearDamp(0.4f); body.set_angularDamp(0.6f);
        body.set_rollResistance(0.1f);
    }

    if (_e.name == "CheesePhys") {
        body.add_CylinderShape(0u, 0.5f, 0.35f, 40.f);
        body.set_bounciness(0.2f); body.set_friction(0.6f);
        body.set_linearDamp(0.1f); body.set_angularDamp(0.2f);
    }
}

template<> void World::update_component(DynamicBodyComponent* _c, EntityRPG* _e) {
    auto* transform = _e->get_component<TransformComponent>();
    transform->PROP_position = maths::mix(_c->prevPosition, _c->body.get_position(), tickPercent);
    transform->PROP_rotation = maths::mix(_c->prevRotation, _c->body.get_rotation(), tickPercent);
    this->mark_refresh_component<TransformComponent>  (_e, true);
    this->mark_refresh_component<ModelComponent>      (_e, true);
    this->mark_refresh_component<DecalComponent>      (_e, true);
    this->mark_refresh_component<SpotLightComponent>  (_e, true);
    this->mark_refresh_component<PointLightComponent> (_e, true);
    this->mark_refresh_component<ReflectComponent>    (_e, true);
}

template<> void World::tick_component(DynamicBodyComponent* _c, EntityRPG* _e) {
    _c->prevPosition = _c->body.get_position();
    _c->prevRotation = _c->body.get_rotation();

    if (_e->name == "CheesePhys") {
        Vec3F difference = get_Camera().PROP_position + get_Camera().PROP_direction*2.f - _c->prevPosition;
        _c->body.apply_force_centre(Vec3F(0.f, 0.f, _c->body.get_mass()) + difference*200.f);
        _c->body.set_linearDamp(0.9f);
    }
}

}
