#pragma once

#include <sqee/app/Resources.hpp>
#include <sqee/maths/Volumes.hpp>
#include <sqee/ecs/Entity.hpp>

#include <sqee/gl/Textures.hpp>
#include <sqee/render/Mesh.hpp>

#include "../../resources/Material.hpp"
#include "../../resources/Manager.hpp"

#include "../../components/Transform.hpp"
#include "../../components/Model.hpp"

namespace sqt { namespace render {

struct ModelSimpleData {

    ModelSimpleData(const sq::Entity& _entity);

    void refresh();

    // references to component data
    const TransformComponent& cTransform;
    const ModelComponent& cModel;

    // data required for rendering
    vector<HandleMaterial> mtrls; HandleMesh mesh;
    Mat4F modelMat; sq::BoundBox bbox;
    bool hasMaskTexture, mirror;
};

}}
