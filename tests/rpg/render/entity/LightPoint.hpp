/*#pragma once

#include <sqee/app/Resources.hpp>
#include <sqee/maths/Volumes.hpp>
#include <sqee/ecs/Entity.hpp>

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>

#include "../../components/Transform.hpp"
#include "../../components/LightPoint.hpp"

namespace sqt { namespace render {

struct LightPointData {

    LightPointData(const sq::Entity& _entity);

    void refresh();

    // references to component data
    const TransformComponent& cTransform;
    const LightPointComponent& cLightPoint;
    const sq::Entity* const entityID;

    // data required for rendering
    struct Face { sq::FrameBuffer fbo; sq::Frustum frus; Mat4F matrix; };
    sq::UniformBuffer ubo; sq::TextureCube tex; Face faces[6];
    sq::Sphere sphere; Mat4F modelMat;
};

}}
*/
