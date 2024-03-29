#include <sqee/gl/Context.hpp>
#include <sqee/debug/Misc.hpp>

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

#include "Renderer.hpp"

using namespace sqt;

//============================================================================//

Renderer::~Renderer() = default;

//============================================================================//

Renderer::Renderer(const Options& options) : options(options)
{
    mRenderStuff = std::make_unique<RenderStuff>();

    mPassesData = std::make_unique<render::PassesData>();

    volumes = std::make_unique<render::StencilVolumes>();
    textures = std::make_unique<render::TargetTextures>();

    //-- allocate pass drawing objects -----------------------//

    render::SharedStuff shared { options, *volumes, *textures, mProcessor, sq::Context::get() };

    depthDraw = std::make_unique<render::DepthPasses>(shared);
    gbufferDraw = std::make_unique<render::GbufferPasses>(shared);
    shadowsDraw = std::make_unique<render::ShadowsPasses>(shared);
    lightBaseDraw = std::make_unique<render::LightBasePasses>(shared);
    lightAccumDraw = std::make_unique<render::LightAccumPasses>(shared);
    volumetricDraw = std::make_unique<render::VolumetricPasses>(shared);
    compositeDraw = std::make_unique<render::CompositePasses>(shared);
    effectsDraw = std::make_unique<render::EffectsPasses>(shared);
}

//============================================================================//

void Renderer::refresh_options()
{
    std::string headerStr = "\n// set of constants and defines added at runtime\n";

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

    headerStr += "\n// some handy shortcuts for comman use of this data\n"
                 "const float OPTION_Aspect = float(OPTION_WinWidth) / float(OPTION_WinHeight);\n"
                 "const vec2 OPTION_WinSizeFull = vec2(OPTION_WinWidth, OPTION_WinHeight);\n"
                 "const vec2 OPTION_WinSizeHalf = round(OPTION_WinSizeFull / 2.f);\n"
                 "const vec2 OPTION_WinSizeQter = round(OPTION_WinSizeFull / 4.f);\n"
                 "const vec2 OPTION_PixSizeFull = 1.0f / OPTION_WinSizeFull;\n"
                 "const vec2 OPTION_PixSizeHalf = 1.0f / OPTION_WinSizeHalf;\n"
                 "const vec2 OPTION_PixSizeQter = 1.0f / OPTION_WinSizeQter;\n";

    #ifdef SQEE_DEBUG

    if (sqeeDebugToggle1) headerStr += "#define DEBUG_TOGGLE_1\n";
    if (sqeeDebugToggle2) headerStr += "#define DEBUG_TOGGLE_2\n";

    #endif

    mProcessor.import_header("runtime/Options", headerStr);

    //--------------------------------------------------------//

    *textures = render::TargetTextures(options);

    depthDraw->update_options();
    gbufferDraw->update_options();
    shadowsDraw->update_options();
    lightBaseDraw->update_options();
    lightAccumDraw->update_options();
    volumetricDraw->update_options();
    compositeDraw->update_options();
    effectsDraw->update_options();
}

//============================================================================//

void Renderer::render_scene(const WorldStuff& wstuff)
{
    auto& context = sq::Context::get();

    // TODO: find a proper solution for this
    mPassesData->gbufferData.modelPasses.simplePass.baseVec.reserve(256u);
    mPassesData->gbufferData.modelPasses.skellyPass.baseVec.reserve(32u);

    //-- prepare all the rendering data ----------------------//

    refresh_render_stuff(*mRenderStuff, wstuff, options);

    mPassesData->prepare(wstuff, *mRenderStuff);

    //-- setup some default context state --------------------//

    context.bind_program_default();
    context.bind_buffer(mRenderStuff->camera.ubo, sq::BufTarget::Uniform, 0u);
    context.set_depth_compare(sq::CompareFunc::LessEqual);

    //-- finally render the stuff ----------------------------//

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
