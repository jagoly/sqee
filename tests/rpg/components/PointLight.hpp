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

class PointLightComponent : public sq::Component {
public:
    PointLightComponent();

    static string type() { return "PointLight"; }

    void setup_depends(sq::Entity* _e);

    // User Properties /////
    Vec3F PROP_colour  = {1.f, 1.f, 1.f};
    uint  PROP_texsize = 0u;

    // System Properties /////
    sq::TextureCube tex;
    sq::UniformBuffer ubo;
    array<sq::Frustum, 6> frusArr;
    array<Mat4F, 6> matArr;
    sq::Sphere sphere;
    Mat4F modelMat;

    // Hard Depended Components /////
    const TransformComponent* DEP_Transform;
};

}
