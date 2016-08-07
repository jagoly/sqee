#pragma once

#include <sqee/gl/FrameBuffer.hpp>

#include "../SharedStuff.hpp"
#include "../PassesData.hpp"

namespace sqt { namespace render {

class VolumetricPasses final : public SharedStuff {
public:
    VolumetricPasses(const SharedStuff& _stuff);

    void update_options();

    void render(const data::VolumetricPasses& _data);

private:

    sq::FrameBuffer FB_Shafts, FB_HalfDepthBlit;

    sq::Texture2D TEX_Shafts_Depth {gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, sq::Texture::NearestClamp()};

    sq::Shader VS_Shafts_Stencil {gl::VERTEX_SHADER};
    sq::Shader VS_Shafts_Light {gl::VERTEX_SHADER};

    sq::Shader FS_Shafts_LightCasc {gl::FRAGMENT_SHADER};
    sq::Shader FS_Shafts_LightOrtho {gl::FRAGMENT_SHADER};
    sq::Shader FS_Shafts_LightPoint {gl::FRAGMENT_SHADER};
    sq::Shader FS_Shafts_LightSpot {gl::FRAGMENT_SHADER};

    void impl_render_SkyLightPass(const data::VolumetricSkyLightPass& _light);

    void impl_render_StencilPass(const data::VolumetricStencilPass& _light,
                                 const sq::Volume& _volume, const sq::Shader& _shader);
};

}}
