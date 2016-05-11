#include "Transform.hpp"

using namespace sqt;

void TransformComponent::setup_depends(sq::Entity* _e) {
    if (sq::Entity* parent = _e->get_parent()) {
        if (auto pcTransform = parent->get_component<TransformComponent>())
            pcTransform->rDepends.push_back(this), PRNT_Transform = pcTransform;
    }
}
