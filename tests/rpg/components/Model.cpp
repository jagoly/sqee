#include <sqee/app/Resources.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/render/Armature.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>

#include "../wcoe/World.hpp"
#include "Transform.hpp"
#include "Model.hpp"

namespace sqt {
namespace maths = sq::maths;

template<> void World::configure_component(ModelComponent* _c, sq::Entity* _e) {
    if ((_c->mesh = sq::static_Mesh().get(_c->PROP_mesh)) == nullptr)
        _c->mesh = sq::static_Mesh().add(_c->PROP_mesh, _c->PROP_mesh);
    if ((_c->skin = sq::static_Skin().get(_c->PROP_skin)) == nullptr)
        _c->skin = sq::static_Skin().add(_c->PROP_skin, _c->PROP_skin);

    if (_c->PROP_arma.empty() == false) {
        if ((_c->arma = sq::static_Armature().get(_c->PROP_arma)) == nullptr)
            _c->arma = sq::static_Armature().add(_c->PROP_arma, _c->PROP_arma);

        if (_c->arma->boneVector.size() * 12u != _c->ubo.get_size() - 32u) {
            _c->ubo.delete_object();
            _c->ubo.reserve("matrix", 16u);
            _c->ubo.reserve("normMat", 16u);
            _c->ubo.reserve("bones", _c->arma->boneVector.size() * 12u);
            _c->ubo.create_and_allocate();
        }
    } else if (_c->ubo.get_size() != 32u) {
        _c->ubo.delete_object();
        _c->ubo.reserve("matrix", 16u);
        _c->ubo.reserve("normMat", 16u);
        _c->ubo.create_and_allocate();
        _c->arma = nullptr;
    }
}

template<> void World::refresh_component(ModelComponent* _c, sq::Entity* _e) {
    const auto transform = _e->get_component<TransformComponent>();

    _c->matrix = maths::scale(transform->matrix, _c->PROP_scale);
    _c->bbox = sq::make_BoundBox(_c->matrix, _c->mesh->origin, _c->mesh->radius, _c->mesh->bbsize);
    _c->negScale = maths::determinant(_c->matrix) < 0.f;

    _c->ubo.update("matrix", &_c->matrix);

    if (_c->arma != nullptr) {
        const auto uboData = sq::Armature::make_UboData(_c->arma->poseMap.at(_c->PROP_pose));
        _c->ubo.update("bones", uboData.data());
    }
}

template<> void World::update_component(ModelComponent* _c, sq::Entity* _e) {
    _c->normMat = Mat4F(maths::transpose(maths::inverse(Mat3F(camera.viewMat * _c->matrix))));

    _c->ubo.update("normMat", &_c->normMat);
}

}
