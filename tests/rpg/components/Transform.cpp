#include "../wcoe/World.hpp"
#include "Transform.hpp"

namespace maths = sq::maths;

namespace sqt {

template<> void World::refresh_component(TransformComponent* _c, sq::Entity* _e) {
    _c->matrix = maths::translate(Mat4F(), _c->PROP_position);
    _c->matrix *= Mat4F(Mat3F(_c->PROP_rotation) * Mat3F(_c->PROP_scale));

    if (const auto parent = _e->get_parent())
        if (const auto pc = parent->get_component<TransformComponent>())
            _c->matrix = pc->matrix * _c->matrix;
}

}
