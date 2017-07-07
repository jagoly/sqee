#include <map>
#include <set>

#include <sqee/assert.hpp>
#include <sqee/gl/Context.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/misc/StringCast.hpp>

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


void Renderer::render_scene(const WorldStuff& wstuff)
{
    static auto& context = Context::get();

    // TODO: find a proper solution for this
    mPassesData->gbufferData.modelPasses.simplePass.baseVec.reserve(256u);
    mPassesData->gbufferData.modelPasses.skellyPass.baseVec.reserve(32u);

    //--------------------------------------------------------//

    refresh_render_stuff(*mRenderStuff, wstuff, options);

    mPassesData->prepare(wstuff, *mRenderStuff);

    //--------------------------------------------------------//

    context.bind_Program_default();

    // setup some state
    context.bind_UniformBuffer(mRenderStuff->camera.ubo, 0u);
    context.set_state(Context::Depth_Compare::LessEqual);

    shadowsDraw->render(mPassesData->shadowsData);

    depthDraw->render(mPassesData->depthData);
    gbufferDraw->render(mPassesData->gbufferData);

    effectsDraw->render_effect_SSAO();

    lightBaseDraw->render(mPassesData->lightBaseData);
    lightAccumDraw->render(mPassesData->lightAccumData);

    effectsDraw->render_effect_Bloom();

    volumetricDraw->render(mPassesData->volumetricData);

    effectsDraw->render_effect_Shafts();

//    reflects->render_reflections();
//    renderer->render_particles();

    compositeDraw->render_to_screen();

    effectsDraw->render_effect_Overlay();
}



Renderer::Renderer(const Options& options) : options(options)
{
    mRenderStuff = std::make_unique<RenderStuff>();

    // Allocate Shared Resource Structs /////
    volumes = std::make_unique<render::StencilVolumes>();
    textures = std::make_unique<render::TargetTextures>();

    render::SharedStuff shared { *volumes, *textures, options, mProcessor, Context::get() };

    // Import GLSL Headers /////
    mProcessor.import_header("headers/blocks/Camera");
    mProcessor.import_header("headers/blocks/SkyBox");
    mProcessor.import_header("headers/blocks/Ambient");
    mProcessor.import_header("headers/blocks/Skeleton");
    mProcessor.import_header("headers/blocks/LightCasc");
    mProcessor.import_header("headers/blocks/LightOrtho");
    mProcessor.import_header("headers/blocks/LightPoint");
    mProcessor.import_header("headers/blocks/LightSpot");
    mProcessor.import_header("headers/blocks/reflect");
    mProcessor.import_header("headers/shadow/sample_casc");
    mProcessor.import_header("headers/shadow/sample_ortho");
    mProcessor.import_header("headers/shadow/sample_point");
    mProcessor.import_header("headers/shadow/sample_spot");

    // Allocate Passes Drawing Objects /////
    depthDraw = std::make_unique<render::DepthPasses>(shared);
    gbufferDraw = std::make_unique<render::GbufferPasses>(shared);
    shadowsDraw = std::make_unique<render::ShadowsPasses>(shared);
    lightBaseDraw = std::make_unique<render::LightBasePasses>(shared);
    lightAccumDraw = std::make_unique<render::LightAccumPasses>(shared);
    volumetricDraw = std::make_unique<render::VolumetricPasses>(shared);
    compositeDraw = std::make_unique<render::CompositePasses>(shared);
    effectsDraw = std::make_unique<render::EffectsPasses>(shared);

    mPassesData = std::make_unique<render::PassesData>();
}


void Renderer::refresh_options()
{
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

    mProcessor.update_header("runtime/Options", headerStr);

    textures->update_options(options);

    depthDraw->update_options();
    gbufferDraw->update_options();
    shadowsDraw->update_options();
    lightBaseDraw->update_options();
    lightAccumDraw->update_options();
    volumetricDraw->update_options();
    compositeDraw->update_options();
    effectsDraw->update_options();
}
