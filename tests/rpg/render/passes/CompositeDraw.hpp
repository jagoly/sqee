#pragma once

#include <sqee/gl/FrameBuffer.hpp>

#include "../SharedStuff.hpp"

namespace sqt { namespace render {

//============================================================================//

class CompositePasses : public SharedStuff
{
public:

    //========================================================//

    CompositePasses(const SharedStuff& stuff);

    void update_options();

    void render_to_screen();

private:

    //========================================================//

    sq::FrameBuffer FB_Composite;

    sq::Shader FS_Composite {sq::Shader::Stage::Fragment};
    sq::Shader FS_FSAA_Screen {sq::Shader::Stage::Fragment};
};

//============================================================================//

}} // namespace sqt::render
