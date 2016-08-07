#pragma once

#include <sqee/gl/Textures.hpp>
#include <sqee/gl/UniformBuffer.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Volumes.hpp>

#include "Helpers.hpp"

namespace sqt {

class PointLightComponent : public ComponentRPG
                          , public ecs::TagConfigure
                          , public ecs::TagRefresh {
public:
    PointLightComponent(EntityRPG&, World&);
    static string type() { return "PointLight"; }

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
};

}
