#include <sqee/redist/gl_ext_4_2.hpp>

#include "Transform.hpp"
#include "SpotLight.hpp"

using namespace sqt;

SpotLightComponent::SpotLightComponent() :
    tex(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, sq::Texture::ShadowMap()) {

    ubo.reserve("direction", 3u);
    ubo.reserve("intensity", 1u);
    ubo.reserve("position", 3u);
    ubo.reserve("softness", 1u);
    ubo.reserve("colour", 3u);
    ubo.reserve("angle", 1u);
    ubo.reserve("matrix", 16u);
    ubo.create_and_allocate();
}

void SpotLightComponent::setup_depends(sq::Entity* _e) {
    auto cTransform = _e->get_component<TransformComponent>();
    SQASSERT(cTransform != nullptr, "'SpotLight' is dependant on 'Transform'");
    cTransform->rDepends.push_back(this); DEP_Transform = cTransform;
}

