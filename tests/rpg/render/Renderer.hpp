#pragma once

#include <sqee/builtins.hpp>
#include <sqee/app/MessageBus.hpp>

#include "../messages.hpp"

namespace sqt {

class SceneData;

namespace render {

class StencilVolumes;
class TargetTextures;
class GenericShaders;

class DepthPasses;
class GbufferPasses;
class ShadowsPasses;
class LightBasePasses;
class LightAccumPasses;
class VolumetricPasses;
class CompositePasses;
class EffectsPasses;

}

class Renderer final {

public:
    Renderer(sq::MessageBus& _messageBus);

    ~Renderer();

    void update_options();
    void render_scene(const SceneData& _scene);

    float tickPercent = 0.f;

private:

    sq::MessageBus& messageBus;

    sq::Receiver<msg::Enable_SkyBox> on_Enable_SkyBox;
    sq::Receiver<msg::Enable_Ambient> on_Enable_Ambient;
    sq::Receiver<msg::Enable_SkyLight> on_Enable_SkyLight;

    sq::Receiver<msg::Disable_SkyBox> on_Disable_SkyBox;
    sq::Receiver<msg::Disable_Ambient> on_Disable_Ambient;
    sq::Receiver<msg::Disable_SkyLight> on_Disable_SkyLight;

    sq::Receiver<msg::Create_Entity> on_Create_Entity;
    sq::Receiver<msg::Configure_Entity> on_Configure_Entity;
    sq::Receiver<msg::Destroy_Entity> on_Destroy_Entity;

    unique_ptr<render::StencilVolumes> volumes;
    unique_ptr<render::TargetTextures> textures;
    unique_ptr<render::GenericShaders> shaders;

    unique_ptr<render::DepthPasses> depthDraw;
    unique_ptr<render::GbufferPasses> gbufferDraw;
    unique_ptr<render::ShadowsPasses> shadowsDraw;
    unique_ptr<render::LightBasePasses> lightBaseDraw;
    unique_ptr<render::LightAccumPasses> lightAccumDraw;
    unique_ptr<render::VolumetricPasses> volumetricDraw;
    unique_ptr<render::CompositePasses> compositeDraw;
    unique_ptr<render::EffectsPasses> effectsDraw;

    struct Impl;
    friend struct Impl;
    unique_ptr<Impl> impl;
};

}
