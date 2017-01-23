#include <map>
#include <set>

#include <sqee/assert.hpp>
#include <sqee/gl/Context.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/misc/StringCast.hpp>

#include "../world/World.hpp"

#include "../world/objects/Camera.hpp"
#include "../world/objects/SkyBox.hpp"
#include "../world/objects/Ambient.hpp"
#include "../world/objects/SkyLight.hpp"

#include "../systems/Entity.hpp"

#include "ObjectsData.hpp"
#include "PassesData.hpp"
#include "SharedStuff.hpp"

#include "passes/DepthDraw.hpp"
#include "passes/GbufferDraw.hpp"
#include "passes/ShadowsDraw.hpp"
#include "passes/LightBaseDraw.hpp"
#include "passes/LightAccumDraw.hpp"
#include "passes/VolumetricDraw.hpp"
#include "passes/CompositeDraw.hpp"
#include "passes/EffectsDraw.hpp"

#include "../Options.hpp"
#include "Renderer.hpp"

using namespace sqt;
using Context = sq::Context;
namespace maths = sq::maths;

Renderer::~Renderer() = default;

struct Renderer::Impl {
    Impl(Renderer& _renderer) : renderer(_renderer) {}

    void refresh_entity_data(const SceneData& _scene);
    void refresh_world_data(const SceneData& _scene);

    Renderer& renderer;

    render::ObjectsData objectsData;
    render::PassesData passesData;
};

void Renderer::Impl::refresh_entity_data(const SceneData& _scene)
{
    //auto& entitySys = sys::static_EntitySystem();

    //for (auto& entry : dop::entries(objectsData.mModelSimpleTable))
    {
        //if (entitySys.check_dirty(entry.id)) configure_entity(entry.id);
    }

    //for (auto& entry : dop::entries(objectsData.mModelSimpleTable))
    {
        //if (ent.first->check_dirty() == false) continue;
        //entry->refresh(entry.id);
        //if (ent.second.modelSkelly) ent.second.modelSkelly->refresh();
        //if (ent.second.decalBasic) ent.second.decalBasic->refresh();
        //if (ent.second.lightOrtho) ent.second.lightOrtho->refresh();
        //if (ent.second.lightPoint) ent.second.lightPoint->refresh();
        //if (ent.second.lightSpot) ent.second.lightSpot->refresh();
    }
}

void Renderer::Impl::refresh_world_data(const SceneData& _scene) {
    objectsData.cameraData->refresh(_scene);
    if (objectsData.skyboxData) objectsData.skyboxData->refresh(*_scene.skybox);
    if (objectsData.ambientData) objectsData.ambientData->refresh(*_scene.ambient);
    if (objectsData.skylightData) objectsData.skylightData->refresh(_scene);
}




void Renderer::render_scene(const SceneData& scene)
{
    static auto& context = Context::get();

    // TODO: find a proper solution for this
    impl->passesData.gbufferData.modelPasses.simplePass.baseVec.reserve(256u);
    impl->passesData.gbufferData.modelPasses.skellyPass.baseVec.reserve(32u);

    impl->refresh_entity_data(scene);
    impl->refresh_world_data(scene);

    render::refresh_render_tables(impl->objectsData);

    impl->passesData.prepare(impl->objectsData);


    // setup some state
    context.bind_UniformBuffer(impl->objectsData.cameraData->ubo, 0u);
    context.set_state(Context::Depth_Compare::LessEqual);

    context.bind_shader_pipeline();

    shadowsDraw->render(impl->passesData.shadowsData);

    depthDraw->render(impl->passesData.depthData);
    gbufferDraw->render(impl->passesData.gbufferData);

    effectsDraw->render_effect_SSAO();

    lightBaseDraw->render(impl->passesData.lightBaseData);
    lightAccumDraw->render(impl->passesData.lightAccumData);

    effectsDraw->render_effect_Bloom();

    volumetricDraw->render(impl->passesData.volumetricData);

    effectsDraw->render_effect_Shafts();

//    reflects->render_reflections();
//    renderer->render_particles();

    compositeDraw->render_to_screen();

    effectsDraw->render_effect_Overlay();
}



Renderer::Renderer(sq::MessageBus& _messageBus) : messageBus(_messageBus), impl(new Impl(*this)) {

    //recConfigureEntity.func = [this](EntityRPG& _e) { this->configure_entity(_e); };
    //recRefreshEntity.func = [this](EntityRPG& _e) { this->refresh_entity(_e); };

    //QuatF quat(0.0767f, 0.0654f, 0.f);
    //Vec3F vec = quat * Vec3F(0.f, 0.f, -1.f);
    //std::cout << sq::chai_string(vec) << std::endl;

    impl->objectsData.cameraData = std::make_unique<render::CameraData>();

    on_Enable_SkyBox.func = [this](auto) { impl->objectsData.skyboxData = std::make_unique<render::SkyBoxData>(); };
    on_Enable_Ambient.func = [this](auto) { impl->objectsData.ambientData = std::make_unique<render::AmbientData>(); };
    on_Enable_SkyLight.func = [this](auto) { impl->objectsData.skylightData = std::make_unique<render::SkyLightData>(); };

    on_Disable_SkyBox.func = [this](auto) { impl->objectsData.skyboxData.reset(nullptr); };
    on_Disable_Ambient.func = [this](auto) { impl->objectsData.ambientData.reset(nullptr); };
    on_Disable_SkyLight.func = [this](auto) { impl->objectsData.skylightData.reset(nullptr); };

    messageBus.subscribe_back(on_Enable_SkyBox);
    messageBus.subscribe_back(on_Enable_Ambient);
    messageBus.subscribe_back(on_Enable_SkyLight);

    messageBus.subscribe_back(on_Disable_SkyBox);
    messageBus.subscribe_back(on_Disable_Ambient);
    messageBus.subscribe_back(on_Disable_SkyLight);

    // Allocate Shared Resource Structs /////
    volumes = std::make_unique<render::StencilVolumes>();
    textures = std::make_unique<render::TargetTextures>();
    shaders = std::make_unique<render::GenericShaders>();

    // Import GLSL Headers /////
    shaders->preprocs.import_header("headers/blocks/Camera");
    shaders->preprocs.import_header("headers/blocks/SkyBox");
    shaders->preprocs.import_header("headers/blocks/Ambient");
    shaders->preprocs.import_header("headers/blocks/Skeleton");
    shaders->preprocs.import_header("headers/blocks/LightCasc");
    shaders->preprocs.import_header("headers/blocks/LightOrtho");
    shaders->preprocs.import_header("headers/blocks/LightPoint");
    shaders->preprocs.import_header("headers/blocks/LightSpot");
    shaders->preprocs.import_header("headers/blocks/reflect");
    shaders->preprocs.import_header("headers/shadow/sample_casc");
    shaders->preprocs.import_header("headers/shadow/sample_ortho");
    shaders->preprocs.import_header("headers/shadow/sample_point");
    shaders->preprocs.import_header("headers/shadow/sample_spot");

    // Allocate Passes Drawing Objects /////
    render::SharedStuff stuff { *volumes, *textures, *shaders, Options::get(), Context::get() };

    depthDraw = std::make_unique<render::DepthPasses>(stuff);
    gbufferDraw = std::make_unique<render::GbufferPasses>(stuff);
    shadowsDraw = std::make_unique<render::ShadowsPasses>(stuff);
    lightBaseDraw = std::make_unique<render::LightBasePasses>(stuff);
    lightAccumDraw = std::make_unique<render::LightAccumPasses>(stuff);
    volumetricDraw = std::make_unique<render::VolumetricPasses>(stuff);
    compositeDraw = std::make_unique<render::CompositePasses>(stuff);
    effectsDraw = std::make_unique<render::EffectsPasses>(stuff);
}


void Renderer::update_options()
{
    static const auto& options = Options::get();

    string headerStr = "// set of constants and defines added at runtime\n";

    headerStr += "const uint  OPTION_WinWidth  = " + std::to_string(options.Window_Size.x) + ";\n";
    headerStr += "const uint  OPTION_WinHeight = " + std::to_string(options.Window_Size.y) + ";\n";
    headerStr += "const float OPTION_ViewNear  = " + std::to_string(                 0.1f) + ";\n";
    headerStr += "const float OPTION_ViewFar   = " + std::to_string(options.View_Distance) + ";\n";

    if (options.Shadows_Large  == true) headerStr += "#define OPTION_SHADOWS_LARGE\n";
    if (options.Shadows_Filter == true) headerStr += "#define OPTION_SHADOWS_FILTER\n";
    if (options.Bloom_Enable   == true) headerStr += "#define OPTION_BLOOM_ENABLE\n";
    if (options.Shafts_Quality != 0u)   headerStr += "#define OPTION_SHAFTS_ENABLE\n";
    if (options.SSAO_Quality   != 0u)   headerStr += "#define OPTION_SSAO_ENABLE\n";
    if (options.FSAA_Quality   != 0u)   headerStr += "#define OPTION_FSAA_ENABLE\n";
    if (options.Shafts_Quality >= 2u)   headerStr += "#define OPTION_SHAFTS_HIGH\n";
    if (options.SSAO_Quality   >= 2u)   headerStr += "#define OPTION_SSAO_HIGH\n";
    if (options.FSAA_Quality   >= 2u)   headerStr += "#define OPTION_FSAA_HIGH\n";

    headerStr += "// some handy shortcuts for comman use of this data\n"
                 "const float OPTION_Aspect = float(OPTION_WinWidth) / float(OPTION_WinHeight);\n"
                 "const vec2 OPTION_WinSizeFull = vec2(OPTION_WinWidth, OPTION_WinHeight);\n"
                 "const vec2 OPTION_WinSizeHalf = round(OPTION_WinSizeFull / 2.f);\n"
                 "const vec2 OPTION_WinSizeQter = round(OPTION_WinSizeFull / 4.f);\n"
                 "const vec2 OPTION_PixSizeFull = 1.0f / OPTION_WinSizeFull;\n"
                 "const vec2 OPTION_PixSizeHalf = 1.0f / OPTION_WinSizeHalf;\n"
                 "const vec2 OPTION_PixSizeQter = 1.0f / OPTION_WinSizeQter;\n";

    shaders->preprocs.update_header("runtime/Options", headerStr);

    textures->update_options(options);
    shaders->update_options(options);

    depthDraw->update_options();
    gbufferDraw->update_options();
    shadowsDraw->update_options();
    lightBaseDraw->update_options();
    lightAccumDraw->update_options();
    volumetricDraw->update_options();
    compositeDraw->update_options();
    effectsDraw->update_options();
}
