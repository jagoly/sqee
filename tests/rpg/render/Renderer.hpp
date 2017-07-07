#pragma once

#include <sqee/builtins.hpp>
#include <sqee/app/MessageBus.hpp>
#include <sqee/app/PreProcessor.hpp>

#include "../messages.hpp"

namespace sqt {

class WorldStuff;
class RenderStuff;

class Options;

namespace render {

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

} // namespace render

class Renderer final
{
public:

    Renderer(const Options& options);

    ~Renderer();

    void refresh_options();
    void render_scene(const WorldStuff& wstuff);

    float tickPercent = 0.f;

private:

    sq::PreProcessor mProcessor;

    unique_ptr<RenderStuff> mRenderStuff;

    unique_ptr<render::StencilVolumes> volumes;
    unique_ptr<render::TargetTextures> textures;

    unique_ptr<render::DepthPasses> depthDraw;
    unique_ptr<render::GbufferPasses> gbufferDraw;
    unique_ptr<render::ShadowsPasses> shadowsDraw;
    unique_ptr<render::LightBasePasses> lightBaseDraw;
    unique_ptr<render::LightAccumPasses> lightAccumDraw;
    unique_ptr<render::VolumetricPasses> volumetricDraw;
    unique_ptr<render::CompositePasses> compositeDraw;
    unique_ptr<render::EffectsPasses> effectsDraw;

    unique_ptr<render::PassesData> mPassesData;

    const Options& options;
};

} // namespace sqt
