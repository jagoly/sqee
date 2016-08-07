#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/General.hpp>

#include "StaticModel.hpp"

using namespace sqt::render;
namespace maths = sq::maths;

StaticModelData::StaticModelData(const world::StaticModel& _model) {

    mtrls.resize(_model.PROP_mtrls.size());
    mtrls.shrink_to_fit(); hasMaskTexture = false;

    for (uint i = 0u; i < mtrls.size(); ++i) {
        mtrls[i] = acquire_Material(_model.PROP_mtrls[i]);
        hasMaskTexture |= mtrls[i]->mask.check(); }

    mesh = acquire_Mesh(_model.PROP_mesh);

    modelMat = maths::translate(Mat4F(), _model.PROP_position);
    modelMat = modelMat * Mat4F(Mat3F(_model.PROP_rotation));
    modelMat = maths::scale(modelMat, _model.PROP_scale);

    mirror = std::signbit(maths::determinant(modelMat));
    bbox = mesh->create_BoundBox(modelMat);
}
