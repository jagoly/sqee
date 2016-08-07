#pragma once

#include <sqee/app/Resources.hpp>
#include <sqee/maths/Vectors.hpp>

#include <sqee/gl/Textures.hpp>

#include "../../world/objects/SkyBox.hpp"

namespace sqt { namespace render {

struct SkyBoxData {

    SkyBoxData();

    // update object from world data
    void refresh(const world::SkyBoxObject& _obj);

    // data required for rendering
    sq::TextureCube tex; string path; Vec4F params;
};

}}
