#include <sqee/gl/Context.hpp>

#include "CompositeDraw.hpp"

using namespace sqt::render;

//============================================================================//

CompositePasses::CompositePasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    processor.load_vertex(PROG_Composite, "shaders/generic/FullScreen_vs.glsl", {});
    processor.load_vertex(PROG_FSAA_Screen, "shaders/generic/FullScreen_vs.glsl", {});
}

//============================================================================//

void CompositePasses::update_options()
{
    processor.load_fragment(PROG_Composite, "shaders/composite/Composite_fs.glsl", {});

    if (options.FSAA_Quality) processor.load_fragment(PROG_FSAA_Screen, "shaders/composite/FSAA/FXAA_fs.glsl", {});
    else processor.load_fragment(PROG_FSAA_Screen, "shaders/generic/PassThrough_fs.glsl", {});

    PROG_Composite.link_program_stages();
    PROG_FSAA_Screen.link_program_stages();

    // re-attach texture to composite framebuffer
    FB_Composite.attach(sq::FboAttach::Colour0, textures.Composite_Main);
}

//============================================================================//

void CompositePasses::render_to_screen()
{
    context.set_state(sq::CullFace::Disable);
    context.set_state(sq::DepthTest::Disable);
    context.set_state(sq::StencilTest::Disable);
    context.set_state(sq::BlendMode::Disable);

    context.set_ViewPort(options.Window_Size);

    context.bind_vertexarray_dummy();

    context.bind_texture(textures.Lighting_Main, 0u);

    if (options.Shafts_Quality != 0u)
    {
        // bind textures required for light shafts
        context.bind_texture(textures.Volumetric_Shafts, 1u);
        context.bind_texture(textures.Depth_HalfSize, 2u);
        context.bind_texture(textures.Depth_FullSize, 3u);
    }

    if (options.Bloom_Enable == true)
    {
        // bind texture required for bloom effect
        context.bind_texture(textures.Effects_Bloom, 4u);
    }

    context.bind_framebuffer(FB_Composite, sq::FboTarget::Both);
    context.bind_program(PROG_Composite);

    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);

    context.bind_framebuffer_default(sq::FboTarget::Both);
    context.bind_texture(textures.Composite_Main, 0u);

    context.bind_program(PROG_FSAA_Screen);

    if (options.Debug_Texture.empty() == false)
    {
        // bind a different texture for debugging if enabled
        if (options.Debug_Texture == "diffuse") context.bind_texture(textures.Gbuffer_MainDiff, 0u);
        if (options.Debug_Texture == "surface") context.bind_texture(textures.Gbuffer_MainSurf, 0u);
        if (options.Debug_Texture == "normals") context.bind_texture(textures.Gbuffer_MainNorm, 0u);
        if (options.Debug_Texture == "specular") context.bind_texture(textures.Gbuffer_MainSpec, 0u);
        if (options.Debug_Texture == "lighting") context.bind_texture(textures.Lighting_Main, 0u);
        if (options.Debug_Texture == "volumetric") context.bind_texture(textures.Volumetric_Shafts, 0u);
        if (options.Debug_Texture == "ssao") context.bind_texture(textures.Effects_SSAO, 0u);
        if (options.Debug_Texture == "bloom") context.bind_texture(textures.Effects_Bloom, 0u);
    }

    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);
}
