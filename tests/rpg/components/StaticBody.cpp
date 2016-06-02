#include <rp3d/engine/DynamicsWorld.hpp>

#include "../wcoe/World.hpp"
#include "StaticBody.hpp"
#include "Transform.hpp"

namespace sqt {
namespace maths = sq::maths;

StaticBodyComponent::StaticBodyComponent(EntityRPG& _e, World& _w) : body(_w.get_PhysicsWorld()) {
    if (_e.name == "FloorPhys") {
        body.add_BoxShape(0u, Vec3F(16.35f, 7.25f, 1.f), Vec3F(0.f, 0.f, -1.f), {});
        body.set_bounciness(0.f); body.set_friction(0.3f);
    }
}

template<> void World::refresh_component(StaticBodyComponent* _c, EntityRPG* _e) {
    const Mat4F& transformMat = _e->get_component<TransformComponent>()->matrix;
    _c->body.set_transform(Vec3F(transformMat[3]), QuatF(Mat3F(transformMat)));
}

}
