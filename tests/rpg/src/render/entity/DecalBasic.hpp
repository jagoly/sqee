/*#pragma once

#include <sqee/app/Resources.hpp>
#include <sqee/maths/Volumes.hpp>
#include <sqee/ecs/Entity.hpp>

#include <sqee/gl/Textures.hpp>

#include "../../resources/Material.hpp"
#include "../../resources/Manager.hpp"

#include "../../components/Transform.hpp"
#include "../../components/Decal.hpp"

namespace sqt { namespace render {

struct DecalBasicData {

    DecalBasicData(const sq::Entity& _entity);

    void refresh();

    // references to component data
    const TransformComponent& cTransform;
    const DecalComponent& cDecal;
    const sq::Entity* const entityID;

    // data required for rendering
    HandleMaterial material; float alpha;
    Mat4F modelMat; sq::BoundBox bbox;
};

}}
*/
