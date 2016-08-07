#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/General.hpp>

#include "DecalBasic.hpp"

using namespace sqt::render;
namespace maths = sq::maths;

DecalBasicData::DecalBasicData(const sq::Entity& _entity)

    : cTransform(*_entity.get_component<TransformComponent>()),
      cDecal(*_entity.get_component<DecalComponent>()) {
}

void DecalBasicData::refresh() {

    material = acquire_Material(cDecal.PROP_material);

    alpha = maths::clamp(cDecal.PROP_alpha, 0.f, 1.f);
    modelMat = maths::scale(cTransform.matrix, cDecal.PROP_stretch);
    bbox = sq::make_BoundBox(modelMat, Vec3F(0.f), 0.87f, Vec3F(1.f));
}
