#pragma once

#include <sqee/app/Resources.hpp>
#include <sqee/maths/Volumes.hpp>
#include <sqee/render/Mesh.hpp>

#include "../../resources/Material.hpp"
#include "../../resources/Manager.hpp"

#include "../../world/static/StaticStuff.hpp"

namespace sqt { namespace render {

struct StaticModelData {
    StaticModelData(const world::StaticModel& _model);

    vector<HandleMaterial> mtrls; HandleMesh mesh;
    Mat4F modelMat; sq::BoundBox bbox;
    bool hasMaskTexture, mirror;
};

}}
