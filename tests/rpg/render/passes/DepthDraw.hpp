#pragma once

#include <sqee/gl/FrameBuffer.hpp>

#include "../SharedStuff.hpp"
#include "../PassesData.hpp"

namespace sqt { namespace render {

//============================================================================//

class DepthPasses : public SharedStuff
{
public:

    //========================================================//

    DepthPasses(const SharedStuff& stuff);

    void update_options();

    void render(const data::DepthPasses& data);

private:

    //========================================================//

    sq::FrameBuffer FB_FullDepth, FB_HalfDepth, FB_QterDepth;

    sq::Shader VS_DepthPass_SimpleSolid { sq::Shader::Stage::Vertex };
    sq::Shader VS_DepthPass_SkellySolid { sq::Shader::Stage::Vertex };
    sq::Shader VS_DepthPass_SimplePunch { sq::Shader::Stage::Vertex };
    sq::Shader VS_DepthPass_SkellyPunch { sq::Shader::Stage::Vertex };
    sq::Shader FS_DepthPass_ModelPunch { sq::Shader::Stage::Fragment };

    Vec2U INFO_fullSize, INFO_halfSize, INFO_qterSize;

    //========================================================//

    void impl_render_ModelSimplePass(const data::DepthModelSimplePass& data);
    void impl_render_ModelSkellyPass(const data::DepthModelSkellyPass& data);
};

//============================================================================//

}} // namespace sqt::render
