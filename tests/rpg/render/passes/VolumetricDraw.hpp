#pragma once

#include <sqee/gl/FrameBuffer.hpp>

#include "../SharedStuff.hpp"
#include "../PassesData.hpp"

namespace sqt { namespace render {

//============================================================================//

class VolumetricPasses : public SharedStuff
{
public:

    //========================================================//

    VolumetricPasses(const SharedStuff& stuff);

    void update_options();

    void render(const data::VolumetricPasses& data);

private:

    //========================================================//

    sq::FrameBuffer FB_Shafts, FB_HalfDepthBlit;

    sq::Texture2D TEX_Shafts_Depth { sq::Texture::Format::DEP24S8 };

    sq::Shader VS_Shafts_Stencil { sq::Shader::Stage::Vertex };
    sq::Shader VS_Shafts_Light { sq::Shader::Stage::Vertex };

    sq::Shader FS_Shafts_LightCasc { sq::Shader::Stage::Fragment };
    sq::Shader FS_Shafts_LightOrtho { sq::Shader::Stage::Fragment };
    sq::Shader FS_Shafts_LightPoint { sq::Shader::Stage::Fragment };
    sq::Shader FS_Shafts_LightSpot { sq::Shader::Stage::Fragment };

    //========================================================//

    void impl_render_SkyLightPass(const data::VolumetricSkyLightPass& light);

    void impl_render_StencilPass(const data::VolumetricStencilPass& light,
                                 const sq::Volume& volume, const sq::Shader& shader);
};

//============================================================================//

}} // namespace sqt::render
