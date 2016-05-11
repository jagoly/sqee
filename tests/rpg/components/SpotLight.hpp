#pragma once

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Volumes.hpp>

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/Textures.hpp>

#include <sqee/ecs/Component.hpp>
#include <sqee/ecs/Entity.hpp>

namespace sqt {

class TransformComponent;

class SpotLightComponent : public sq::Component {
public:
    SpotLightComponent();

    static string type() { return "SpotLight"; }

    void setup_depends(sq::Entity* _e);

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

    // Hard Depended Components /////
    const TransformComponent* DEP_Transform;
};

}
