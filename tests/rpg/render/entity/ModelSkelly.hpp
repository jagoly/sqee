#pragma once

#include <sqee/app/Resources.hpp>
#include <sqee/maths/Volumes.hpp>
#include <sqee/ecs/Entity.hpp>

#include <sqee/gl/Textures.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/render/Armature.hpp>
#include <sqee/render/Mesh.hpp>

#include "../../resources/Material.hpp"
#include "../../resources/Manager.hpp"

#include "../../components/Transform.hpp"
#include "../../components/Skeleton.hpp"
#include "../../components/Model.hpp"

namespace sqt { namespace render {

struct ModelSkellyData {

    ModelSkellyData(const sq::Entity& _entity);

    void refresh();

    // references to component data
    const TransformComponent& cTransform;
    const SkeletonComponent& cSkeleton;
    const ModelComponent& cModel;

    // data required for rendering
    sq::UniformBuffer ubo; HandleArmature arma;
    vector<HandleMaterial> mtrls; HandleMesh mesh;
    Mat4F modelMat; sq::Sphere sphere;
    bool hasMaskTexture, mirror;
};

}}
