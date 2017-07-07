#pragma once

#include "../SharedStuff.hpp"
#include "../PassesData.hpp"

namespace sqt::render {

//============================================================================//

class LightBasePasses : public SharedStuff
{
public: //====================================================//

    LightBasePasses(const SharedStuff& stuff);

    void update_options();

    void render(const data::LightBasePasses& data);

private: //===================================================//

    sq::FrameBuffer FB_Lighting;

    sq::Program PROG_Main_Skybox;
    sq::Program PROG_Main_Ambient;

    //--------------------------------------------------------//

    void impl_render_SkyBoxPass(const data::LightBaseSkyBoxPass& data);
    void impl_render_AmbientPass(const data::LightBaseAmbientPass& data);
};

//============================================================================//

} // namespace sqt::render
