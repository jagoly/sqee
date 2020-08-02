/*#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/General.hpp>

#include "../../systems/Transform.hpp"

#include "DecalBasic.hpp"

using namespace sqt::render;
namespace maths = sq::maths;

DecalBasicData::DecalBasicData(const sq::Entity& _entity)
    : cTransform(*_entity.get_component<TransformComponent>()),
      cDecal(*_entity.get_component<DecalComponent>()),
      entityID(&_entity)
{}

void DecalBasicData::refresh() {

    material = acquire_Material(cDecal.PROP_material);

    const auto tf = sys_Transform().get_transform(entityID);

    // calculate decal model matrix and bounding box
    modelMat = maths::scale(tf.matrix, cDecal.PROP_stretch);
    bbox = sq::make_BoundBox(modelMat, Vec3F(0.f), 0.87f, Vec3F(1.f));

    // clamp alpha value to normalised range
    alpha = maths::clamp(cDecal.PROP_alpha, 0.f, 1.f);
}
*/
