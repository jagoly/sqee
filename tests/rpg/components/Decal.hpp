#pragma once

#include <sqee/forward.hpp>

#include <sqee/maths/Volumes.hpp>

#include <sqee/gl/UniformBuffer.hpp>

#include <sqee/ecs/Component.hpp>
#include <sqee/ecs/Entity.hpp>

namespace sqt {

class TransformComponent;

class DecalComponent : public sq::Component {
public:
    DecalComponent();

    static string type() { return "Decal"; }

    void setup_depends(sq::Entity* _e);

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

    // Hard Depended Components /////
    const TransformComponent* DEP_Transform;
};

}
