#pragma once

#include <sqee/gl/FrameBuffer.hpp>

#include "../SharedStuff.hpp"
#include "../PassesData.hpp"

namespace sqt { namespace render {

class DepthPasses final : public SharedStuff {
public:
    DepthPasses(const SharedStuff& _stuff);

    void update_options();

    void render(const data::DepthPasses& _data);

private:

    sq::FrameBuffer FB_FullDepth, FB_HalfDepth, FB_QterDepth;

    sq::Shader VS_DepthPass_SimpleSolid {gl::VERTEX_SHADER};
    sq::Shader VS_DepthPass_SkellySolid {gl::VERTEX_SHADER};
    sq::Shader VS_DepthPass_SimplePunch {gl::VERTEX_SHADER};
    sq::Shader VS_DepthPass_SkellyPunch {gl::VERTEX_SHADER};
    sq::Shader FS_DepthPass_ModelPunch {gl::FRAGMENT_SHADER};

    Vec2U INFO_fullSize, INFO_halfSize, INFO_qterSize;

    void impl_render_ModelSimplePass(const data::DepthModelSimplePass& _data);
    void impl_render_ModelSkellyPass(const data::DepthModelSkellyPass& _data);
};

}}
