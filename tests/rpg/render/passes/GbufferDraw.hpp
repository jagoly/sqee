#pragma once

#include <sqee/gl/FrameBuffer.hpp>

#include "../SharedStuff.hpp"
#include "../PassesData.hpp"

namespace sqt { namespace render {

class GbufferPasses final : public SharedStuff {
public:
    GbufferPasses(const SharedStuff& _stuff);

    void update_options();

    void render(const data::GbufferPasses& _data);

private:

    sq::FrameBuffer FB_MainGbuffer;

    sq::Shader VS_ModelPass_Simple {gl::VERTEX_SHADER};
    sq::Shader VS_ModelPass_Skelly {gl::VERTEX_SHADER};
    sq::Shader FS_ModelPass_Model {gl::FRAGMENT_SHADER};

    sq::Shader VS_DecalPass_Decal {gl::VERTEX_SHADER};
    sq::Shader FS_DecalPass_Decal {gl::FRAGMENT_SHADER};

    void impl_render_ModelPasses(const data::GbufferModelPasses& _data);
    void impl_render_ModelSimplePass(const data::GbufferModelSimplePass& _data);
    void impl_render_ModelSkellyPass(const data::GbufferModelSkellyPass& _data);

    void impl_render_DecalPasses(const data::GbufferDecalPasses& _data);
    void impl_render_DecalBasicPass(const data::GbufferDecalBasicPass& _data);
};

}}
