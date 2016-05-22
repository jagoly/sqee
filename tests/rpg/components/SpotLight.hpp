#pragma once

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Volumes.hpp>

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/Textures.hpp>

#include <sqee/ecs/Component.hpp>

namespace sqt {

class SpotLightComponent : public sq::Component
                         , public sq::ecs::TagConfigure
                         , public sq::ecs::TagRefresh {
public:
    SpotLightComponent();

    static string type() { return "SpotLight"; }

    // User Properties /////
    Vec3F PROP_colour   = {1.f, 1.f, 1.f};
    float PROP_softness = 0.f;
    float PROP_angle    = 0.125f;
    uint  PROP_texsize  = 0u;

    // System Properties /////
    sq::Texture2D tex;
    sq::UniformBuffer ubo;
    sq::Frustum frus;
    Mat4F modelMat;
    Mat4F matrix;
};

}
