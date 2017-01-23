#pragma once

#include <sqee/gl/FrameBuffer.hpp>

#include "../SharedStuff.hpp"
#include "../PassesData.hpp"

namespace sqt { namespace render {

//============================================================================//

class LightBasePasses : public SharedStuff
{
public:

    //========================================================//

    LightBasePasses(const SharedStuff& stuff);

    void update_options();

    void render(const data::LightBasePasses& data);

private:

    //========================================================//

    sq::FrameBuffer FB_Lighting;

    sq::Shader VS_Main_Skybox { sq::Shader::Stage::Vertex };
    sq::Shader FS_Main_Ambient { sq::Shader::Stage::Fragment };
    sq::Shader FS_Skybox { sq::Shader::Stage::Fragment };

    //========================================================//

    void impl_render_SkyBoxPass(const data::LightBaseSkyBoxPass& data);
    void impl_render_AmbientPass(const data::LightBaseAmbientPass& data);
};

//============================================================================//

}} // namespace sqt::render
