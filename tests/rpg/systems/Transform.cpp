#include "../components/Transform.hpp"

#include "Transform.hpp"

using namespace sqt;
namespace maths = sq::maths;

void TransformSystem::refresh_entity(sq::Entity* _entity) {

    if (auto* cTransform = _entity->try_get_component<TransformComponent>()) {

        cTransform->matrix = Mat4F(); cTransform->matrix[3] = Vec4F(cTransform->PROP_position, 1.f);
        cTransform->matrix *= Mat4F(Mat3F(cTransform->PROP_rotation) * Mat3F(cTransform->PROP_scale));

        if (const auto* parent = _entity->get_parent())
            if (const auto* pTransform = parent->try_get_component<TransformComponent>())
                cTransform->matrix = pTransform->matrix * cTransform->matrix;
    }
}
