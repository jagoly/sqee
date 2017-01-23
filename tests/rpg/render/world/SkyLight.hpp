#pragma once

#include <sqee/misc/Resource.hpp>
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
    struct Cascade { sq::FrameBuffer fbo; sq::maths::Planes<4> planes; Mat4F matrix; };
    sq::UniformBuffer ubo; sq::TextureArray2D tex; Cascade cascades[6];
    uint cascadeCount; float cascadeSize;
};

}}
