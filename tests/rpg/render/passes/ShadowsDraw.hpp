#pragma once

#include "../SharedStuff.hpp"
#include "../PassesData.hpp"

namespace sqt::render {

//============================================================================//

class ShadowsPasses : public SharedStuff
{
public: //====================================================//

    ShadowsPasses(const SharedStuff& stuff);

    void update_options();

    void render(const data::ShadowsPasses& data);

private: //===================================================//

    sq::Program PROG_SimpleSolid;
    sq::Program PROG_SimplePunch;
    sq::Program PROG_SkellySolid;
    sq::Program PROG_SkellyPunch;

    //--------------------------------------------------------//

    void impl_render_ShadowsPass(const data::ShadowsPass& data);
};

//============================================================================//

} // namespace sqt::render
