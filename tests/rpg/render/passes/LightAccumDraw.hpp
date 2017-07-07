#pragma once

#include "../SharedStuff.hpp"
#include "../PassesData.hpp"

namespace sqt::render {

//============================================================================//

class LightAccumPasses : public SharedStuff
{
public: //====================================================//

    LightAccumPasses(const SharedStuff& stuff);

    void update_options();

    void render(const data::LightAccumPasses& data);

private: //===================================================//

    sq::FrameBuffer FB_Lighting;

    sq::Program PROG_Main_Stencil;
    sq::Program PROG_Main_LightCasc;
    sq::Program PROG_Main_LightOrtho_base;
    sq::Program PROG_Main_LightOrtho_shad;
    sq::Program PROG_Main_LightPoint_base;
    sq::Program PROG_Main_LightPoint_shad;
    sq::Program PROG_Main_LightSpot_base;
    sq::Program PROG_Main_LightSpot_shad;

    //sq::Shader FS_defr_refl_spot_none {gl::FRAGMENT_SHADER};
    //sq::Shader FS_defr_refl_spot_shad {gl::FRAGMENT_SHADER};
    //sq::Shader FS_defr_refl_point_none {gl::FRAGMENT_SHADER};
    //sq::Shader FS_defr_refl_point_shad {gl::FRAGMENT_SHADER};

    //--------------------------------------------------------//

    void impl_render_SkyLightPass(const data::LightAccumSkyLightPass& light);

    void impl_render_StencilPass(const data::LightAccumStencilPass& light,
                                 const sq::Volume& volume, const sq::Program& program);
};

//============================================================================//

} // namespace sqt::render
