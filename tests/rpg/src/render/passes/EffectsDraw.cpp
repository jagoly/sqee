#include <sqee/gl/Context.hpp>

#include "EffectsDraw.hpp"

using namespace sqt::render;

//============================================================================//

EffectsPasses::EffectsPasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    FB_SSAO_Main.draw_buffers({sq::FboAttach::Colour0});
    FB_SSAO_Blur.draw_buffers({sq::FboAttach::Colour0});
    FB_Bloom_Main.draw_buffers({sq::FboAttach::Colour0});
    FB_Bloom_Blur.draw_buffers({sq::FboAttach::Colour0});
    FB_Shafts_Main.draw_buffers({sq::FboAttach::Colour0});
    FB_Shafts_Blur.draw_buffers({sq::FboAttach::Colour0});

    processor.load_vertex(PROG_SSAO_Main, "shaders/generic/FullScreen_vs.glsl", {});
    processor.load_vertex(PROG_SSAO_Blur, "shaders/generic/FullScreen_vs.glsl", {});
    processor.load_vertex(PROG_Bloom_Main, "shaders/generic/FullScreen_vs.glsl", {});
    processor.load_vertex(PROG_Bloom_BlurH, "shaders/generic/FullScreen_vs.glsl", {});
    processor.load_vertex(PROG_Bloom_BlurV, "shaders/generic/FullScreen_vs.glsl", {});
    processor.load_vertex(PROG_Shafts_BlurH, "shaders/generic/FullScreen_vs.glsl", {});
    processor.load_vertex(PROG_Shafts_BlurV, "shaders/generic/FullScreen_vs.glsl", {});
    processor.load_vertex(PROG_Overlay, "shaders/generic/FullScreen_vs.glsl", {});
}

//============================================================================//

void EffectsPasses::update_options()
{
    if (options.SSAO_Quality)
    {
        processor.load_fragment(PROG_SSAO_Main, "shaders/effects/SSAO/Main_fs.glsl", {});
        processor.load_fragment(PROG_SSAO_Blur, "shaders/effects/SSAO/Blur_fs.glsl", {});

        PROG_SSAO_Main.link_program_stages();
        PROG_SSAO_Blur.link_program_stages();
    }

    if (options.Bloom_Enable)
    {
        processor.load_fragment(PROG_Bloom_Main, "shaders/effects/Bloom/Main_fs.glsl", {});
        processor.load_fragment(PROG_Bloom_BlurH, "shaders/effects/Bloom/BlurH_fs.glsl", {});
        processor.load_fragment(PROG_Bloom_BlurV, "shaders/effects/Bloom/BlurV_fs.glsl", {});

        PROG_Bloom_Main.link_program_stages();
        PROG_Bloom_BlurH.link_program_stages();
        PROG_Bloom_BlurV.link_program_stages();
    }

    if (options.Shafts_Quality)
    {
        processor.load_fragment(PROG_Shafts_BlurH, "shaders/effects/Shafts/BlurH_fs.glsl", {});
        processor.load_fragment(PROG_Shafts_BlurV, "shaders/effects/Shafts/BlurV_fs.glsl", {});

        PROG_Shafts_BlurH.link_program_stages();
        PROG_Shafts_BlurV.link_program_stages();
    }

    processor.load_fragment(PROG_Overlay, "shaders/effects/Overlay_fs.glsl", {});

    PROG_Overlay.link_program_stages();

    TEX_SSAO_Blur = sq::Texture2D();
    TEX_Bloom_Blur = sq::Texture2D();
    TEX_Shafts_Blur = sq::Texture2D();

    if (options.SSAO_Quality > 0u)
    {
        TEX_SSAO_Blur.allocate_storage(sq::TexFormat::R8_UN, options.Window_Size / 2u, false);
        TEX_SSAO_Blur.set_filter_mode(true, false);
        FB_SSAO_Main.attach(sq::FboAttach::Colour0, textures.Effects_SSAO);
        FB_SSAO_Blur.attach(sq::FboAttach::Colour0, TEX_SSAO_Blur);
    }

    if (options.Bloom_Enable == false)
    {
        TEX_Bloom_Blur.allocate_storage(sq::TexFormat::RGB8_UN, options.Window_Size / 4u, false);
        TEX_Bloom_Blur.set_filter_mode(true, false);
        FB_Bloom_Main.attach(sq::FboAttach::Colour0, textures.Effects_Bloom);
        FB_Bloom_Blur.attach(sq::FboAttach::Colour0, TEX_Bloom_Blur);
    }

    if (options.Shafts_Quality > 0u)
    {
        TEX_Shafts_Blur.allocate_storage(sq::TexFormat::RGB8_UN, options.Window_Size / 2u, false);
        TEX_Shafts_Blur.set_filter_mode(true, false);
        FB_Shafts_Main.attach(sq::FboAttach::Colour0, textures.Volumetric_Shafts);
        FB_Shafts_Blur.attach(sq::FboAttach::Colour0, TEX_Shafts_Blur);
    }
}

//============================================================================//

void EffectsPasses::render_effect_SSAO()
{
    //-- render and blur the ssao effect texture -------------//

    if (!options.SSAO_Quality) return;

    //--------------------------------------------------------//

    context.set_state(sq::CullFace::Disable);
    context.set_state(sq::DepthTest::Disable);
    context.set_state(sq::StencilTest::Disable);
    context.set_state(sq::BlendMode::Disable);

    context.set_ViewPort(options.Window_Size / 2u);

    context.bind_vertexarray_dummy();

    //--------------------------------------------------------//

    context.bind_framebuffer(FB_SSAO_Main, sq::FboTarget::Both);
    context.bind_texture(textures.Depth_HalfSize, 1u);
    context.bind_program(PROG_SSAO_Main);
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);

    //--------------------------------------------------------//

    context.bind_framebuffer(FB_SSAO_Blur, sq::FboTarget::Both);
    context.bind_texture(textures.Effects_SSAO, 0u);
    context.bind_program(PROG_SSAO_Blur);
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);

    context.bind_framebuffer(FB_SSAO_Main, sq::FboTarget::Both);
    context.bind_texture(TEX_SSAO_Blur, 0u);
    context.bind_program(PROG_SSAO_Blur);
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);
}

//============================================================================//

void EffectsPasses::render_effect_Bloom()
{
    //-- render and blur the bloom effect texture ------------//

    if (!options.Bloom_Enable) return;

    //--------------------------------------------------------//

    context.set_state(sq::CullFace::Disable);
    context.set_state(sq::DepthTest::Disable);
    context.set_state(sq::StencilTest::Disable);
    context.set_state(sq::BlendMode::Disable);

    context.set_ViewPort(options.Window_Size / 4u);

    context.bind_vertexarray_dummy();

    //--------------------------------------------------------//

    context.bind_framebuffer(FB_Bloom_Main);
    context.bind_texture(textures.Lighting_Main, 0u);
    context.bind_program(PROG_Bloom_Main);
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);

    //--------------------------------------------------------//

    context.bind_framebuffer(FB_Bloom_Blur);
    context.bind_texture(textures.Effects_Bloom, 0u);
    context.bind_program(PROG_Bloom_BlurH);
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);

    context.bind_framebuffer(FB_Bloom_Main);
    context.bind_texture(TEX_Bloom_Blur, 0u);
    context.bind_program(PROG_Bloom_BlurV);
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);

    context.bind_framebuffer(FB_Bloom_Blur);
    context.bind_texture(textures.Effects_Bloom, 0u);
    context.bind_program(PROG_Bloom_BlurH);
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);

    context.bind_framebuffer(FB_Bloom_Main);
    context.bind_texture(TEX_Bloom_Blur, 0u);
    context.bind_program(PROG_Bloom_BlurV);
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);
}

//============================================================================//

void EffectsPasses::render_effect_Shafts()
{
    //-- blur the volumetric shafts effect texture -----------//

    if (!options.Shafts_Quality) return;

    //--------------------------------------------------------//

    context.set_state(sq::CullFace::Disable);
    context.set_state(sq::DepthTest::Disable);
    context.set_state(sq::StencilTest::Disable);
    context.set_state(sq::BlendMode::Disable);

    context.set_ViewPort(options.Window_Size / 2u);
    context.bind_texture(textures.Depth_HalfSize, 1u);

    context.bind_vertexarray_dummy();

    //--------------------------------------------------------//

    context.bind_framebuffer(FB_Shafts_Blur);
    context.bind_texture(textures.Volumetric_Shafts, 0u);
    context.bind_program(PROG_Shafts_BlurH);
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);

    context.bind_framebuffer(FB_Shafts_Main);
    context.bind_texture(TEX_Shafts_Blur, 0u);
    context.bind_program(PROG_Shafts_BlurV);
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);

    context.bind_framebuffer(FB_Shafts_Blur);
    context.bind_texture(textures.Volumetric_Shafts, 0u);
    context.bind_program(PROG_Shafts_BlurH);
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);

    context.bind_framebuffer(FB_Shafts_Main);
    context.bind_texture(TEX_Shafts_Blur, 0u);
    context.bind_program(PROG_Shafts_BlurV);
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);
}

//============================================================================//

void EffectsPasses::render_effect_Overlay()
{
    //-- render the vignette overlay -------------------------//

    if (!options.Vignette_Enable) return;

    //--------------------------------------------------------//

    context.bind_framebuffer_default();

    context.set_state(sq::CullFace::Disable);
    context.set_state(sq::DepthTest::Disable);
    context.set_state(sq::StencilTest::Disable);
    context.set_state(sq::BlendMode::Alpha);

    context.bind_program(PROG_Overlay);

    context.bind_vertexarray_dummy();
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);
}
