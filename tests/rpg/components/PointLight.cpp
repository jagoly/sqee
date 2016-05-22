#include <sqee/redist/gl_ext_4_2.hpp>

#include "../wcoe/World.hpp"
#include "Transform.hpp"
#include "PointLight.hpp"

namespace sqt {
namespace maths = sq::maths;

PointLightComponent::PointLightComponent() :
    tex(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, sq::Texture::ShadowMap()) {

    ubo.reserve("position", 4u);
    ubo.reserve("colour", 3u);
    ubo.reserve("intensity", 1u);
    ubo.reserve("matArr", 96u);
    ubo.create_and_allocate();
}

template<> void World::configure_component(PointLightComponent* _c, sq::Entity* _e) {
    if (_c->PROP_texsize == 0u) _c->tex.delete_object();
    else { uint newSize = _c->PROP_texsize * (settings.get<bool>("rpg_shadlarge") + 1u);
        if (newSize != _c->tex.get_size().x) _c->tex.allocate_storage(newSize, false); }
}

template<> void World::refresh_component(PointLightComponent* _c, sq::Entity* _e) {
    const auto transform = _e->get_component<TransformComponent>();

    Vec3F position = Vec3F(transform->matrix[3]);
    float intensity = maths::length(Vec3F(transform->matrix[0]));
    _c->modelMat = maths::scale(maths::translate(Mat4F(), position), Vec3F(intensity*2.f));
    _c->sphere.origin = position; _c->sphere.radius = intensity;

    _c->ubo.update("position", &position);
    _c->ubo.update("colour", &_c->PROP_colour);
    _c->ubo.update("intensity", &intensity);

    if (_c->PROP_texsize != 0u) {
        Mat4F projMat = maths::perspective(maths::radians(0.25f), 1.f, 0.1f, intensity);
        _c->matArr[0] = projMat * maths::look_at(position, position+Vec3F(+1.f, 0.f, 0.f), {0.f, -1.f, 0.f});
        _c->matArr[1] = projMat * maths::look_at(position, position+Vec3F(-1.f, 0.f, 0.f), {0.f, -1.f, 0.f});
        _c->matArr[2] = projMat * maths::look_at(position, position+Vec3F(0.f, +1.f, 0.f), {0.f, 0.f, +1.f});
        _c->matArr[3] = projMat * maths::look_at(position, position+Vec3F(0.f, -1.f, 0.f), {0.f, 0.f, -1.f});
        _c->matArr[4] = projMat * maths::look_at(position, position+Vec3F(0.f, 0.f, +1.f), {0.f, -1.f, 0.f});
        _c->matArr[5] = projMat * maths::look_at(position, position+Vec3F(0.f, 0.f, -1.f), {0.f, -1.f, 0.f});
        _c->frusArr[0] = sq::make_Frustum(_c->matArr[0], position, {+1.f, 0.f, 0.f}, 0.1f, intensity);
        _c->frusArr[1] = sq::make_Frustum(_c->matArr[1], position, {-1.f, 0.f, 0.f}, 0.1f, intensity);
        _c->frusArr[2] = sq::make_Frustum(_c->matArr[2], position, {0.f, +1.f, 0.f}, 0.1f, intensity);
        _c->frusArr[3] = sq::make_Frustum(_c->matArr[3], position, {0.f, -1.f, 0.f}, 0.1f, intensity);
        _c->frusArr[4] = sq::make_Frustum(_c->matArr[4], position, {0.f, 0.f, +1.f}, 0.1f, intensity);
        _c->frusArr[5] = sq::make_Frustum(_c->matArr[5], position, {0.f, 0.f, -1.f}, 0.1f, intensity);
        _c->ubo.update("matArr", _c->matArr.data());
    }
}

}
