#pragma once

#include "../SharedStuff.hpp"
#include "../PassesData.hpp"

namespace sqt::render {

//============================================================================//

class GbufferPasses : public SharedStuff
{
public: //====================================================//

    GbufferPasses(const SharedStuff& stuff);

    void update_options();

    void render(const data::GbufferPasses& data);

private: //===================================================//

    sq::FrameBuffer FB_MainGbuffer;

    sq::Program PROG_ModelPass_Simple;
    sq::Program PROG_ModelPass_Skelly;
    sq::Program PROG_DecalPass_Decal;

    //--------------------------------------------------------//

    void impl_render_ModelPasses(const data::GbufferModelPasses& data);
    void impl_render_ModelSimplePass(const data::GbufferModelSimplePass& data);
    void impl_render_ModelSkellyPass(const data::GbufferModelSkellyPass& data);

    void impl_render_DecalPasses(const data::GbufferDecalPasses& data);
    void impl_render_DecalBasicPass(const data::GbufferDecalBasicPass& data);
};

//============================================================================//

} // namespace sqt::render
