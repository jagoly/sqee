#include "Transform.hpp"
#include "Decal.hpp"

using namespace sqt;

DecalComponent::DecalComponent() {
    ubo.reserve("matrix", 16u);
    ubo.reserve("invMat", 16u);
    ubo.reserve("d_n_s", 3u);
    ubo.reserve("alpha", 1u);
    ubo.create_and_allocate();
}

void DecalComponent::setup_depends(sq::Entity* _e) {
    auto cTransform = _e->get_component<TransformComponent>();
    SQASSERT(cTransform != nullptr, "'Decal' is dependant on 'Transform'");
    cTransform->rDepends.push_back(this); DEP_Transform = cTransform;
}
