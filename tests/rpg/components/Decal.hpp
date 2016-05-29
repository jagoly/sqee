#pragma once

#include <sqee/gl/UniformBuffer.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Volumes.hpp>

#include "Helpers.hpp"

namespace sqt {

class DecalComponent : public ComponentRPG
                     , public ecs::TagCleanUp
                     , public ecs::TagConfigure
                     , public ecs::TagRefresh {
public:
    DecalComponent(EntityRPG&, World&);
    static string type() { return "Decal"; }

    // User Properties /////
    Vec3F  PROP_scale = {1.f, 1.f, 1.f};
    float  PROP_alpha = 1.f;
    string PROP_diff  = "";
    string PROP_norm  = "";
    string PROP_spec  = "";

    // System Properties /////
    sq::Texture2D* texDiff;
    sq::Texture2D* texNorm;
    sq::Texture2D* texSpec;
    sq::UniformBuffer ubo;
    sq::BoundBox bbox;
    Mat4F matrix;
    Mat4F invMat;
};

}
