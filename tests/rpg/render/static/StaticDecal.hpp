#pragma once

#include <sqee/app/Resources.hpp>
#include <sqee/maths/Volumes.hpp>

#include "../../resources/Material.hpp"
#include "../../resources/Manager.hpp"

#include "../../world/static/StaticStuff.hpp"

namespace sqt { namespace render {

struct StaticDecalData {
    StaticDecalData(const world::StaticDecal& _decal);

    HandleMaterial material; float alpha;
    Mat4F modelMat; sq::BoundBox bbox;
};

}}
