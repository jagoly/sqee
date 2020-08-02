#pragma once

#include "../SharedStuff.hpp"

//============================================================================//

namespace sqt::render {

class CompositePasses final : public SharedStuff
{
public: //====================================================//

    CompositePasses(const SharedStuff& stuff);

    void update_options();

    void render_to_screen();

private: //===================================================//

    sq::FrameBuffer FB_Composite;

    sq::Program PROG_Composite;
    sq::Program PROG_FSAA_Screen;
};

} // namespace sqt::render
