#pragma once

#include <sqee/app/MessageBus.hpp>
#include <sqee/app/PreProcessor.hpp>

#include "../main/Options.hpp"

//====== Forward Declarations ================================================//

namespace sqt { class RenderStuff; class WorldStuff; }

namespace sqt::render {

struct StencilVolumes;
struct TargetTextures;

class DepthPasses;
class GbufferPasses;
class ShadowsPasses;
class LightBasePasses;
class LightAccumPasses;
class VolumetricPasses;
class CompositePasses;
class EffectsPasses;

class PassesData;

} // namespace sqt::render

//============================================================================//

namespace sqt {

class Renderer final : sq::NonCopyable
{
public: //====================================================//

    Renderer(const Options& options);

    ~Renderer();

    //--------------------------------------------------------//

    void refresh_options();

    void render_scene(const WorldStuff& wstuff);

private: //===================================================//

    const Options& options;

    sq::PreProcessor mProcessor;

    //--------------------------------------------------------//

    std::unique_ptr<RenderStuff> mRenderStuff;

    std::unique_ptr<render::StencilVolumes> volumes;
    std::unique_ptr<render::TargetTextures> textures;

    std::unique_ptr<render::DepthPasses> depthDraw;
    std::unique_ptr<render::GbufferPasses> gbufferDraw;
    std::unique_ptr<render::ShadowsPasses> shadowsDraw;
    std::unique_ptr<render::LightBasePasses> lightBaseDraw;
    std::unique_ptr<render::LightAccumPasses> lightAccumDraw;
    std::unique_ptr<render::VolumetricPasses> volumetricDraw;
    std::unique_ptr<render::CompositePasses> compositeDraw;
    std::unique_ptr<render::EffectsPasses> effectsDraw;

    std::unique_ptr<render::PassesData> mPassesData;

};

} // namespace sqt
