/*#pragma once

#include <sqee/app/Resources.hpp>
#include <sqee/maths/Volumes.hpp>
#include <sqee/ecs/Entity.hpp>

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>

#include "../../components/Transform.hpp"
#include "../../components/LightOrtho.hpp"

namespace sqt { namespace render {

struct LightOrthoData {

    LightOrthoData(const sq::Entity& _entity);

    void refresh();

    // references to component data
    const TransformComponent& cTransform;
    const LightOrthoComponent& cLightOrtho;
    const sq::Entity* const entityID;

    // data required for rendering
    sq::FrameBuffer fbo; sq::Ortho ortho; Mat4F matrix;
    sq::UniformBuffer ubo; sq::Texture2D tex;
    sq::BoundBox bbox; Mat4F modelMat;
};

}}
*/
