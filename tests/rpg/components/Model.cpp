#include "Transform.hpp"
#include "Model.hpp"

using namespace sqt;

void ModelComponent::setup_depends(sq::Entity* _e) {
    auto cTransform = _e->get_component<TransformComponent>();
    SQASSERT(cTransform != nullptr, "'Model' is dependant on 'Transform'");
    cTransform->rDepends.push_back(this), DEP_Transform = cTransform;
}
