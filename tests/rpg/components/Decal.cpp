#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/Resources.hpp>
#include <sqee/gl/Textures.hpp>

#include "../wcoe/World.hpp"
#include "Transform.hpp"
#include "Decal.hpp"

namespace sqt {
namespace maths = sq::maths;

DecalComponent::DecalComponent() {
    ubo.reserve("matrix", 16u);
    ubo.reserve("invMat", 16u);
    ubo.reserve("d_n_s", 3u);
    ubo.reserve("alpha", 1u);
    ubo.create_and_allocate();
}

template<> void World::configure_component(DecalComponent* _c, sq::Entity*) {
    if (_c->PROP_diff.empty() == false) {
        const string path = "decals/" + _c->PROP_diff + "_d";
        if ((_c->texDiff = sq::static_Texture2D().get(path)) == nullptr) {
            _c->texDiff = sq::static_Texture2D().add(path, gl::RGBA, gl::RGBA8, sq::Texture::MipmapClamp());
            _c->texDiff->buffer_auto(path, true); } }
    if (_c->PROP_norm.empty() == false) {
        const string path = "decals/" + _c->PROP_norm + "_n";
        if ((_c->texNorm = sq::static_Texture2D().get(path)) == nullptr) {
            _c->texNorm = sq::static_Texture2D().add(path, gl::RGBA, gl::RGBA8, sq::Texture::MipmapClamp());
            _c->texNorm->buffer_auto(path, true); } }
    if (_c->PROP_spec.empty() == false) {
        const string path = "decals/" + _c->PROP_spec + "_s";
        if ((_c->texSpec = sq::static_Texture2D().get(path)) == nullptr) {
            _c->texSpec = sq::static_Texture2D().add(path, gl::RGBA, gl::RGBA8, sq::Texture::MipmapClamp());
            _c->texSpec->buffer_auto(path, true); } }

    Vec3I d_n_s(bool(_c->texDiff), bool(_c->texNorm), bool(_c->texSpec));
    _c->ubo.update("d_n_s", &d_n_s);
}

template<> void World::refresh_component(DecalComponent* _c, sq::Entity* _e) {
    const auto transform = _e->get_component<TransformComponent>();

    _c->matrix = maths::scale(transform->matrix, _c->PROP_scale);
    _c->bbox = sq::make_BoundBox(_c->matrix, Vec3F(0.f), 0.87f, Vec3F(1.f));
    _c->invMat = maths::inverse(_c->matrix);

    _c->ubo.update("matrix", &_c->matrix);
    _c->ubo.update("invMat", &_c->invMat);
    _c->ubo.update("alpha", &_c->PROP_alpha);
}

}
