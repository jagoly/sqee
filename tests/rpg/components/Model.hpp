#pragma once

#include <sqee/forward.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Volumes.hpp>

#include <sqee/gl/UniformBuffer.hpp>

#include <sqee/ecs/Component.hpp>

namespace sqt {

class ModelComponent : public sq::Component
                     , public sq::ecs::TagConfigure
                     , public sq::ecs::TagRefresh
                     , public sq::ecs::TagUpdate {
public:
    static string type() { return "Model"; }

    // User Properties /////
    Vec3F  PROP_scale  = {1.f, 1.f, 1.f};
    bool   PROP_render = true;
    bool   PROP_shadow = true;
    bool   PROP_decals = true;
    string PROP_mesh   = "";
    string PROP_skin   = "";
    string PROP_arma   = "";
    string PROP_pose   = "";

    // System Properties /////
    sq::UniformBuffer ubo;
    sq::Armature* arma;
    sq::BoundBox bbox;
    sq::Mesh* mesh;
    sq::Skin* skin;
    Mat4F matrix;
    Mat4F normMat;
    bool negScale;
};

}
