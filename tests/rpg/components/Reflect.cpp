#include <sqee/maths/General.hpp>

#include "../wcoe/World.hpp"
#include "Transform.hpp"
#include "Reflect.hpp"

namespace sqt {
namespace maths = sq::maths;

ReflectComponent::ReflectComponent() {
    ubo.reserve("normal", 4u);
    ubo.reserve("trans", 3u);
    ubo.reserve("factor", 1u);
    ubo.create_and_allocate();
}

template<> void World::refresh_component(ReflectComponent* _c, sq::Entity* _e) {
    const auto transform = _e->get_component<TransformComponent>();

    _c->normal = maths::normalize(sq::make_normMat(transform->matrix) * Vec3F(0.f, 0.f, 1.f));
    _c->trans = Vec3F(transform->matrix[3]);

    _c->ubo.update("normal", &_c->normal);
    _c->ubo.update("trans", &_c->trans);
    _c->ubo.update("factor", &_c->PROP_factor);
}

}
