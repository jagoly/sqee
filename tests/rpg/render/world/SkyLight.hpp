#pragma once

#include <sqee/app/Resources.hpp>
#include <sqee/maths/Volumes.hpp>

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>

#include "../../world/World.hpp"

namespace sqt { namespace render {

struct SkyLightData {

    SkyLightData();

    void refresh(const SceneData& _scene);

    // data required for rendering
    struct Cascade { sq::FrameBuffer fbo; sq::Ortho ortho; Mat4F matrix; };
    sq::UniformBuffer ubo; sq::Texture2DArray tex; Cascade cascades[6];
    uint cascadeCount; float cascadeSize;
};

}}
