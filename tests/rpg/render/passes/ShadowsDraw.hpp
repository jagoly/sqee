#pragma once

#include <sqee/gl/FrameBuffer.hpp>

#include "../SharedStuff.hpp"
#include "../PassesData.hpp"

namespace sqt { namespace render {

class ShadowsPasses final : public SharedStuff {
public:
    ShadowsPasses(const SharedStuff& _stuff);

    void update_options();

    void render(const data::ShadowsPasses& _data);

private:

    sq::Shader VS_SimpleSolid {gl::VERTEX_SHADER};
    sq::Shader VS_SkellySolid {gl::VERTEX_SHADER};
    sq::Shader VS_SimplePunch {gl::VERTEX_SHADER};
    sq::Shader VS_SkellyPunch {gl::VERTEX_SHADER};
    sq::Shader FS_ModelPunch {gl::FRAGMENT_SHADER};

    void impl_render_ShadowsPass(const data::ShadowsPass& _data);
};

}}
