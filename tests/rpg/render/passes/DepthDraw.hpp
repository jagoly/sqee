#pragma once

#include "../SharedStuff.hpp"
#include "../PassesData.hpp"

namespace sqt::render {

//============================================================================//

class DepthPasses : public SharedStuff
{
public: //====================================================//

    DepthPasses(const SharedStuff& stuff);

    void update_options();

    void render(const data::DepthPasses& data);

private: //===================================================//

    sq::FrameBuffer FB_FullDepth, FB_HalfDepth, FB_QterDepth;

    sq::Program PROG_DepthPass_SimpleSolid;
    sq::Program PROG_DepthPass_SimplePunch;
    sq::Program PROG_DepthPass_SkellySolid;
    sq::Program PROG_DepthPass_SkellyPunch;

    Vec2U INFO_fullSize, INFO_halfSize, INFO_qterSize;

    //--------------------------------------------------------//

    void impl_render_ModelSimplePass(const data::DepthModelSimplePass& data);
    void impl_render_ModelSkellyPass(const data::DepthModelSkellyPass& data);
};

//============================================================================//

} // namespace sqt::render
