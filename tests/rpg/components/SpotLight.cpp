#include <sqee/redist/gl_ext_4_2.hpp>

#include "../RpgApp.hpp"
#include "../wcoe/World.hpp"
#include "Transform.hpp"
#include "SpotLight.hpp"

namespace sqt {
namespace maths = sq::maths;

SpotLightComponent::SpotLightComponent(EntityRPG&, World&) :
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

template<> void World::configure_component(SpotLightComponent* _c, EntityRPG* _e) {
    if (_c->PROP_texsize == 0u) _c->tex.delete_object();
    else { uint newSize = _c->PROP_texsize * (options.ShadowLarge + 1u);
        if (newSize != _c->tex.get_size().x) _c->tex.allocate_storage(Vec2U(newSize), false); }
}

template<> void World::refresh_component(SpotLightComponent* _c, EntityRPG* _e) {
    const auto transform = _e->get_component<TransformComponent>();

    Vec3F position = Vec3F(transform->matrix[3]);
    Vec3F direction = maths::normalize(Mat3F(transform->matrix) * Vec3F(0.f, 0.f, -1.f));
    Vec3F tangent = maths::normalize(Mat3F(transform->matrix) * Vec3F(0.f, 1.f, 0.f));
    Mat4F viewMat = maths::look_at(position, position + direction, tangent);

    float angle = maths::radians(_c->PROP_angle);
    float intensity = maths::length(Vec3F(transform->matrix[0]));
    _c->matrix = maths::perspective(angle*2.f, 1.f, 0.2f, intensity) * viewMat;
    _c->frus = sq::make_Frustum(_c->matrix, position, direction, 0.2f, intensity);
    Vec3F modelScale = Vec3F(Vec2F(-std::tan(angle*2.f)), -1.f) * intensity;
    _c->modelMat = maths::scale(maths::inverse(viewMat), modelScale);

    _c->ubo.update("direction", &direction);
    _c->ubo.update("intensity", &intensity);
    _c->ubo.update("position", &position);
    _c->ubo.update("softness", &_c->PROP_softness);
    _c->ubo.update("colour", &_c->PROP_colour);
    _c->ubo.update("angle", &angle);
    _c->ubo.update("matrix", &_c->matrix);
}

}
