#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/General.hpp>

#include "ModelSimple.hpp"

using namespace sqt::render;
namespace maths = sq::maths;

ModelSimpleData::ModelSimpleData(const sq::Entity& _entity)

    : cTransform(*_entity.get_component<TransformComponent>()),
      cModel(*_entity.get_component<ModelComponent>()) {
}


void ModelSimpleData::refresh() {

    mtrls.resize(cModel.PROP_mtrls.size());
    mtrls.shrink_to_fit(); hasMaskTexture = false;

    for (uint i = 0u; i < mtrls.size(); ++i) {
        mtrls[i] = acquire_Material(cModel.PROP_mtrls[i]);
        hasMaskTexture |= mtrls[i]->mask.check(); }

    mesh = acquire_Mesh(cModel.PROP_mesh);

    modelMat = maths::scale(cTransform.matrix, cModel.PROP_stretch);
    mirror = std::signbit(maths::determinant(modelMat));
    bbox = mesh->create_BoundBox(modelMat);
}
