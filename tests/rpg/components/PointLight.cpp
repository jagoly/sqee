#include <sqee/redist/gl_ext_4_2.hpp>

#include "Transform.hpp"
#include "PointLight.hpp"

using namespace sqt;

PointLightComponent::PointLightComponent() :
    tex(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, sq::Texture::ShadowMap()) {

    ubo.reserve("position", 4u);
    ubo.reserve("colour", 3u);
    ubo.reserve("intensity", 1u);
    ubo.reserve("matArr", 96u);
    ubo.create_and_allocate();
}

void PointLightComponent::setup_depends(sq::Entity* _e) {
    auto cTransform = _e->get_component<TransformComponent>();
    SQASSERT(cTransform != nullptr, "'PointLight' is dependant on 'Transform'");
    cTransform->rDepends.push_back(this); DEP_Transform = cTransform;
}

