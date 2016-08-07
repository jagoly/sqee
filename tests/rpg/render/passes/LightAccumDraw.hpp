#pragma once

#include <sqee/gl/FrameBuffer.hpp>

#include "../SharedStuff.hpp"
#include "../PassesData.hpp"

namespace sqt { namespace render {

class LightAccumPasses final : public SharedStuff {
public:
    LightAccumPasses(const SharedStuff& _stuff);

    void update_options();

    void render(const data::LightAccumPasses& _data);

private:

    sq::FrameBuffer FB_Lighting;

    sq::Shader VS_Main_Stencil {gl::VERTEX_SHADER};

    sq::Shader FS_Main_LightCasc_shad {gl::FRAGMENT_SHADER};
    sq::Shader FS_Main_LightOrtho_base {gl::FRAGMENT_SHADER};
    sq::Shader FS_Main_LightOrtho_shad {gl::FRAGMENT_SHADER};
    sq::Shader FS_Main_LightPoint_base {gl::FRAGMENT_SHADER};
    sq::Shader FS_Main_LightPoint_shad {gl::FRAGMENT_SHADER};
    sq::Shader FS_Main_LightSpot_base {gl::FRAGMENT_SHADER};
    sq::Shader FS_Main_LightSpot_shad {gl::FRAGMENT_SHADER};

    //sq::Shader FS_defr_refl_spot_none {gl::FRAGMENT_SHADER};
    //sq::Shader FS_defr_refl_spot_shad {gl::FRAGMENT_SHADER};
    //sq::Shader FS_defr_refl_point_none {gl::FRAGMENT_SHADER};
    //sq::Shader FS_defr_refl_point_shad {gl::FRAGMENT_SHADER};

    void impl_render_SkyLightPass(const data::LightAccumSkyLightPass& _light);

    void impl_render_StencilPass(const data::LightAccumStencilPass& _light,
                                 const sq::Volume& _volume, const sq::Shader& _shader);
};

}}
