#pragma once

#include <sqee/gl/FrameBuffer.hpp>

#include "../SharedStuff.hpp"

namespace sqt { namespace render {

class CompositePasses final : public SharedStuff {
public:
    CompositePasses(const SharedStuff& _stuff);

    void update_options();

    void render_to_screen();

private:

    sq::FrameBuffer FB_Composite;

    sq::Shader FS_Composite {gl::FRAGMENT_SHADER};
    sq::Shader FS_FSAA_Screen {gl::FRAGMENT_SHADER};
};

}}
