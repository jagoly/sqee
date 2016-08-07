#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/General.hpp>

#include "StaticDecal.hpp"

using namespace sqt::render;
namespace maths = sq::maths;

StaticDecalData::StaticDecalData(const world::StaticDecal& _decal) {
    material = acquire_Material(_decal.PROP_material);
    alpha = maths::clamp(_decal.PROP_alpha, 0.f, 1.f);

    modelMat = maths::translate(Mat4F(), _decal.PROP_position);
    modelMat = modelMat * Mat4F(Mat3F(_decal.PROP_rotation));
    modelMat = maths::scale(modelMat, _decal.PROP_scale);

    bbox = sq::make_BoundBox(modelMat, Vec3F(0.f), 0.87f, Vec3F(1.f));
}
