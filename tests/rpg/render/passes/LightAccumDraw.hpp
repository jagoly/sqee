#pragma once

#include <sqee/gl/FrameBuffer.hpp>

#include "../SharedStuff.hpp"
#include "../PassesData.hpp"

namespace sqt { namespace render {

//============================================================================//

class LightAccumPasses : public SharedStuff
{
public:

    //========================================================//

    LightAccumPasses(const SharedStuff& stuff);

    void update_options();

    void render(const data::LightAccumPasses& data);

private:

    //========================================================//

    sq::FrameBuffer FB_Lighting;

    sq::Shader VS_Main_Stencil { sq::Shader::Stage::Vertex };

    sq::Shader FS_Main_LightCasc_shad { sq::Shader::Stage::Fragment };
    sq::Shader FS_Main_LightOrtho_base { sq::Shader::Stage::Fragment };
    sq::Shader FS_Main_LightOrtho_shad { sq::Shader::Stage::Fragment };
    sq::Shader FS_Main_LightPoint_base { sq::Shader::Stage::Fragment };
    sq::Shader FS_Main_LightPoint_shad { sq::Shader::Stage::Fragment };
    sq::Shader FS_Main_LightSpot_base { sq::Shader::Stage::Fragment };
    sq::Shader FS_Main_LightSpot_shad { sq::Shader::Stage::Fragment };

    //sq::Shader FS_defr_refl_spot_none {gl::FRAGMENT_SHADER};
    //sq::Shader FS_defr_refl_spot_shad {gl::FRAGMENT_SHADER};
    //sq::Shader FS_defr_refl_point_none {gl::FRAGMENT_SHADER};
    //sq::Shader FS_defr_refl_point_shad {gl::FRAGMENT_SHADER};

    //========================================================//

    void impl_render_SkyLightPass(const data::LightAccumSkyLightPass& light);

    void impl_render_StencilPass(const data::LightAccumStencilPass& light,
                                 const sq::Volume& volume, const sq::Shader& shader);
};

//============================================================================//

}} // namespace sqt::render
