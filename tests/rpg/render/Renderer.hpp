#pragma once

#include <sqee/misc/Builtins.hpp>

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

    UniquePtr<RenderStuff> mRenderStuff;

    UniquePtr<render::StencilVolumes> volumes;
    UniquePtr<render::TargetTextures> textures;

    UniquePtr<render::DepthPasses> depthDraw;
    UniquePtr<render::GbufferPasses> gbufferDraw;
    UniquePtr<render::ShadowsPasses> shadowsDraw;
    UniquePtr<render::LightBasePasses> lightBaseDraw;
    UniquePtr<render::LightAccumPasses> lightAccumDraw;
    UniquePtr<render::VolumetricPasses> volumetricDraw;
    UniquePtr<render::CompositePasses> compositeDraw;
    UniquePtr<render::EffectsPasses> effectsDraw;

    UniquePtr<render::PassesData> mPassesData;

};

} // namespace sqt
