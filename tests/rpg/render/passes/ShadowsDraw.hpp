#pragma once

#include <sqee/gl/FrameBuffer.hpp>

#include "../SharedStuff.hpp"
#include "../PassesData.hpp"

namespace sqt { namespace render {

//============================================================================//

class ShadowsPasses : public SharedStuff
{
public:

    //========================================================//

    ShadowsPasses(const SharedStuff& stuff);

    void update_options();

    void render(const data::ShadowsPasses& data);

private:

    //========================================================//

    sq::Shader VS_SimpleSolid { sq::Shader::Stage::Vertex };
    sq::Shader VS_SkellySolid { sq::Shader::Stage::Vertex };
    sq::Shader VS_SimplePunch { sq::Shader::Stage::Vertex };
    sq::Shader VS_SkellyPunch { sq::Shader::Stage::Vertex };
    sq::Shader FS_ModelPunch { sq::Shader::Stage::Fragment };

    //========================================================//

    void impl_render_ShadowsPass(const data::ShadowsPass& data);
};

//============================================================================//

}} // namespace sqt::render
