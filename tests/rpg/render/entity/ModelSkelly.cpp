#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/General.hpp>

#include "ModelSkelly.hpp"

using namespace sqt::render;
namespace maths = sq::maths;

ModelSkellyData::ModelSkellyData(const sq::Entity& _entity)

    : cTransform(*_entity.get_component<TransformComponent>()),
      cSkeleton(*_entity.get_component<SkeletonComponent>()),
      cModel(*_entity.get_component<ModelComponent>()) {

    ubo.reserve("bones", 960u);
    ubo.create_and_allocate();
}


void ModelSkellyData::refresh() {

    mtrls.resize(cModel.PROP_mtrls.size());
    mtrls.shrink_to_fit(); hasMaskTexture = false;

    for (uint i = 0u; i < mtrls.size(); ++i) {
        mtrls[i] = acquire_Material(cModel.PROP_mtrls[i]);
        hasMaskTexture |= mtrls[i]->mask.check(); }

    arma = acquire_Armature(cSkeleton.PROP_armature);
    mesh = acquire_Mesh(cModel.PROP_mesh);

    modelMat = maths::scale(cTransform.matrix, cModel.PROP_stretch);
    mirror = std::signbit(maths::determinant(modelMat));
    sphere = {Vec3F(modelMat[3]), 2.f};

    const auto& pose = arma->poseMap.at(cSkeleton.PROP_pose);
    auto uboData = sq::Armature::make_UboData(pose);
    ubo.update("bones", uboData.data());
}
