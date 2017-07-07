#include <sqee/gl/Context.hpp>
#include <sqee/gl/Drawing.hpp>

#include "../../Options.hpp"
#include "CompositeDraw.hpp"

using Context = sq::Context;
using namespace sqt::render;

//============================================================================//

CompositePasses::CompositePasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    processor.load_vertex(PROG_Composite, "generic/FullScreen_vs");
    processor.load_vertex(PROG_FSAA_Screen, "generic/FullScreen_vs");
}

//============================================================================//

void CompositePasses::update_options()
{
    processor.load_fragment(PROG_Composite, "composite/Composite_fs");

    if (options.FSAA_Quality) processor.load_fragment(PROG_FSAA_Screen, "composite/FSAA/FXAA_fs");
    else processor.load_fragment(PROG_FSAA_Screen, "generic/PassThrough_fs");

    PROG_Composite.link_program_stages();
    PROG_FSAA_Screen.link_program_stages();

    // re-attach texture to composite framebuffer
    FB_Composite.attach(gl::COLOR_ATTACHMENT0, textures.Composite_Main);
}

//============================================================================//

void CompositePasses::render_to_screen()
{
    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Disable);

    context.set_ViewPort(options.Window_Size);

    context.bind_Texture(textures.Lighting_Main, 0u);

    if (options.Shafts_Quality != 0u)
    {
        // bind textures required for light shafts
        context.bind_Texture(textures.Volumetric_Shafts, 1u);
        context.bind_Texture(textures.Depth_HalfSize, 2u);
        context.bind_Texture(textures.Depth_FullSize, 3u);
    }

    if (options.Bloom_Enable == true)
    {
        // bind texture required for bloom effect
        context.bind_Texture(textures.Effects_Bloom, 4u);
    }

    context.bind_FrameBuffer(FB_Composite);
    context.bind_Program(PROG_Composite);
    sq::draw_screen_quad();

    context.bind_FrameBuffer_default();
    context.bind_Texture(textures.Composite_Main, 0u);

    context.bind_Program(PROG_FSAA_Screen);

    if (options.Debug_Texture.empty() == false)
    {
        // bind a different texture for debugging if enabled
        if (options.Debug_Texture == "diffuse") context.bind_Texture(textures.Gbuffer_MainDiff, 0u);
        if (options.Debug_Texture == "surface") context.bind_Texture(textures.Gbuffer_MainSurf, 0u);
        if (options.Debug_Texture == "normals") context.bind_Texture(textures.Gbuffer_MainNorm, 0u);
        if (options.Debug_Texture == "specular") context.bind_Texture(textures.Gbuffer_MainSpec, 0u);
        if (options.Debug_Texture == "lighting") context.bind_Texture(textures.Lighting_Main, 0u);
        if (options.Debug_Texture == "volumetric") context.bind_Texture(textures.Volumetric_Shafts, 0u);
        if (options.Debug_Texture == "ssao") context.bind_Texture(textures.Effects_SSAO, 0u);
        if (options.Debug_Texture == "bloom") context.bind_Texture(textures.Effects_Bloom, 0u);
    }

    sq::draw_screen_quad();

    context.bind_Program_default();
}

//============================================================================//
