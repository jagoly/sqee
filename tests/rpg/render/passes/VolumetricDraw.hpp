#pragma once

#include "../SharedStuff.hpp"
#include "../PassesData.hpp"

namespace sqt::render {

//============================================================================//

class VolumetricPasses : public SharedStuff
{
public: //====================================================//

    VolumetricPasses(const SharedStuff& stuff);

    void update_options();

    void render(const data::VolumetricPasses& data);

private: //===================================================//

    sq::FrameBuffer FB_Shafts, FB_HalfDepthBlit;

    sq::Texture2D TEX_Shafts_Depth { sq::Texture::Format::DEP24S8 };

    sq::Program PROG_Shafts_Stencil;
    sq::Program PROG_Shafts_LightCasc;
    sq::Program PROG_Shafts_LightOrtho;
    sq::Program PROG_Shafts_LightPoint;
    sq::Program PROG_Shafts_LightSpot;

    //--------------------------------------------------------//

    void impl_render_SkyLightPass(const data::VolumetricSkyLightPass& light);

    void impl_render_StencilPass(const data::VolumetricStencilPass& light,
                                 const sq::Volume& volume, const sq::Program& program);
};

//============================================================================//

} // namespace sqt::render
