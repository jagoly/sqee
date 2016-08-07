#pragma once

#include <sqee/gl/FrameBuffer.hpp>

#include "../SharedStuff.hpp"
#include "../PassesData.hpp"

namespace sqt { namespace render {

class LightBasePasses final : public SharedStuff {
public:
    LightBasePasses(const SharedStuff& _stuff);

    void update_options();

    void render(const data::LightBasePasses& _data);

private:

    sq::FrameBuffer FB_Lighting;

    sq::Shader VS_Main_Skybox {gl::VERTEX_SHADER};
    sq::Shader FS_Main_Ambient {gl::FRAGMENT_SHADER};
    sq::Shader FS_Skybox {gl::FRAGMENT_SHADER};

    void impl_render_SkyBoxPass(const data::LightBaseSkyBoxPass& _data);
    void impl_render_AmbientPass(const data::LightBaseAmbientPass& _data);
};

}}
