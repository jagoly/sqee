#pragma once

#include <sqee/gl/FrameBuffer.hpp>

#include "../SharedStuff.hpp"
#include "../PassesData.hpp"

namespace sqt { namespace render {

//============================================================================//

class GbufferPasses : public SharedStuff
{
public:

    //========================================================//

    GbufferPasses(const SharedStuff& stuff);

    void update_options();

    void render(const data::GbufferPasses& data);

private:

    //========================================================//

    sq::FrameBuffer FB_MainGbuffer;

    sq::Shader VS_ModelPass_Simple { sq::Shader::Stage::Vertex };
    sq::Shader VS_ModelPass_Skelly { sq::Shader::Stage::Vertex };
    sq::Shader FS_ModelPass_Model { sq::Shader::Stage::Fragment };

    sq::Shader VS_DecalPass_Decal { sq::Shader::Stage::Vertex };
    sq::Shader FS_DecalPass_Decal { sq::Shader::Stage::Fragment };

    //========================================================//

    void impl_render_ModelPasses(const data::GbufferModelPasses& data);
    void impl_render_ModelSimplePass(const data::GbufferModelSimplePass& data);
    void impl_render_ModelSkellyPass(const data::GbufferModelSkellyPass& data);

    void impl_render_DecalPasses(const data::GbufferDecalPasses& data);
    void impl_render_DecalBasicPass(const data::GbufferDecalBasicPass& data);
};

//============================================================================//

}} // namespace sqt::render
