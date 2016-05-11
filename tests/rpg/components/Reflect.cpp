#include "Transform.hpp"
#include "Model.hpp"
#include "Reflect.hpp"

using namespace sqt;

ReflectComponent::ReflectComponent() {
    ubo.reserve("normal", 4u);
    ubo.reserve("trans", 3u);
    ubo.reserve("factor", 1u);
    ubo.create_and_allocate();
}

void ReflectComponent::setup_depends(sq::Entity* _e) {
    auto cModel = _e->get_component<ModelComponent>();
    SQASSERT(cModel != nullptr, "'Reflect' is dependant on 'Model'");
    DEP_Transform = _e->get_component<TransformComponent>();
    cModel->rDepends.push_back(this); DEP_Model = cModel;
}
