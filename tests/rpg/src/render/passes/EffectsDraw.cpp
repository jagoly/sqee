#include <sqee/gl/Context.hpp>
#include <sqee/gl/Drawing.hpp>

#include <sqee/redist/gl_loader.hpp>

#include "EffectsDraw.hpp"

using Context = sq::Context;
using namespace sqt::render;

//============================================================================//

EffectsPasses::EffectsPasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    FB_SSAO_Main.draw_buffers({gl::COLOR_ATTACHMENT0});
    FB_SSAO_Blur.draw_buffers({gl::COLOR_ATTACHMENT0});
    FB_Bloom_Main.draw_buffers({gl::COLOR_ATTACHMENT0});
    FB_Bloom_Blur.draw_buffers({gl::COLOR_ATTACHMENT0});
    FB_Shafts_Main.draw_buffers({gl::COLOR_ATTACHMENT0});
    FB_Shafts_Blur.draw_buffers({gl::COLOR_ATTACHMENT0});

    TEX_SSAO_Blur.set_filter_mode(true);
    TEX_Bloom_Blur.set_filter_mode(true);
    TEX_Shafts_Blur.set_filter_mode(true);

    processor.load_vertex(PROG_SSAO_Main, "generic/FullScreen_vs");
    processor.load_vertex(PROG_SSAO_Blur, "generic/FullScreen_vs");
    processor.load_vertex(PROG_Bloom_Main, "generic/FullScreen_vs");
    processor.load_vertex(PROG_Bloom_BlurH, "generic/FullScreen_vs");
    processor.load_vertex(PROG_Bloom_BlurV, "generic/FullScreen_vs");
    processor.load_vertex(PROG_Shafts_BlurH, "generic/FullScreen_vs");
    processor.load_vertex(PROG_Shafts_BlurV, "generic/FullScreen_vs");
    processor.load_vertex(PROG_Overlay, "generic/FullScreen_vs");
}

//============================================================================//

void EffectsPasses::update_options()
{
    if (options.SSAO_Quality)
    {
        processor.load_fragment(PROG_SSAO_Main, "effects/SSAO/Main_fs");
        processor.load_fragment(PROG_SSAO_Blur, "effects/SSAO/Blur_fs");

        PROG_SSAO_Main.link_program_stages();
        PROG_SSAO_Blur.link_program_stages();
    }

    if (options.Bloom_Enable)
    {
        processor.load_fragment(PROG_Bloom_Main, "effects/Bloom/Main_fs");
        processor.load_fragment(PROG_Bloom_BlurH, "effects/Bloom/BlurH_fs");
        processor.load_fragment(PROG_Bloom_BlurV, "effects/Bloom/BlurV_fs");

        PROG_Bloom_Main.link_program_stages();
        PROG_Bloom_BlurH.link_program_stages();
        PROG_Bloom_BlurV.link_program_stages();
    }

    if (options.Shafts_Quality)
    {
        processor.load_fragment(PROG_Shafts_BlurH, "effects/Shafts/BlurH_fs");
        processor.load_fragment(PROG_Shafts_BlurV, "effects/Shafts/BlurV_fs");

        PROG_Shafts_BlurH.link_program_stages();
        PROG_Shafts_BlurV.link_program_stages();
    }

    processor.load_fragment(PROG_Overlay, "effects/Overlay_fs");

    PROG_Overlay.link_program_stages();

    // delete or allocate ssao blur texture
    if (options.SSAO_Quality == 0u) TEX_SSAO_Blur.delete_object();
    else TEX_SSAO_Blur.allocate_storage(options.Window_Size / 2u);

    // delete or allocate bloom blur texture
    if (options.Bloom_Enable == false) TEX_Bloom_Blur.delete_object();
    else TEX_Bloom_Blur.allocate_storage(options.Window_Size / 4u);

    // delete or allocate shafts blur texture
    if (options.Shafts_Quality == 0u) TEX_Shafts_Blur.delete_object();
    else TEX_Shafts_Blur.allocate_storage(options.Window_Size / 2u);

    // re-attach textures to ssao main and blur framebuffers
    if (options.SSAO_Quality) FB_SSAO_Main.attach(gl::COLOR_ATTACHMENT0, textures.Effects_SSAO);
    if (options.SSAO_Quality) FB_SSAO_Blur.attach(gl::COLOR_ATTACHMENT0, TEX_SSAO_Blur);

    // re-attach textures to bloom main and blur framebuffers
    if (options.Bloom_Enable) FB_Bloom_Main.attach(gl::COLOR_ATTACHMENT0, textures.Effects_Bloom);
    if (options.Bloom_Enable) FB_Bloom_Blur.attach(gl::COLOR_ATTACHMENT0, TEX_Bloom_Blur);

    // re-attach textures to shafts main and blur framebuffers
    if (options.Shafts_Quality) FB_Shafts_Main.attach(gl::COLOR_ATTACHMENT0, textures.Volumetric_Shafts);
    if (options.Shafts_Quality) FB_Shafts_Blur.attach(gl::COLOR_ATTACHMENT0, TEX_Shafts_Blur);
}

//============================================================================//

void EffectsPasses::render_effect_SSAO()
{
    //-- render and blur the ssao effect texture -------------//

    if (!options.SSAO_Quality) return;

    //--------------------------------------------------------//

    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Disable);

    context.set_ViewPort(options.Window_Size / 2u);

    //--------------------------------------------------------//

    context.bind_FrameBuffer(FB_SSAO_Main);
    context.bind_Texture(textures.Depth_HalfSize, 1u);
    context.bind_Program(PROG_SSAO_Main);
    sq::draw_screen_quad();

    //--------------------------------------------------------//

    context.bind_FrameBuffer(FB_SSAO_Blur);
    context.bind_Texture(textures.Effects_SSAO, 0u);
    context.bind_Program(PROG_SSAO_Blur);
    sq::draw_screen_quad();

    context.bind_FrameBuffer(FB_SSAO_Main);
    context.bind_Texture(TEX_SSAO_Blur, 0u);
    context.bind_Program(PROG_SSAO_Blur);
    sq::draw_screen_quad();
}

//============================================================================//

void EffectsPasses::render_effect_Bloom()
{
    //-- render and blur the bloom effect texture ------------//

    if (!options.Bloom_Enable) return;

    //--------------------------------------------------------//

    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Disable);

    context.set_ViewPort(options.Window_Size / 4u);

    //--------------------------------------------------------//

    context.bind_FrameBuffer(FB_Bloom_Main);
    context.bind_Texture(textures.Lighting_Main, 0u);
    context.bind_Program(PROG_Bloom_Main);
    sq::draw_screen_quad();

    //--------------------------------------------------------//

    context.bind_FrameBuffer(FB_Bloom_Blur);
    context.bind_Texture(textures.Effects_Bloom, 0u);
    context.bind_Program(PROG_Bloom_BlurH);
    sq::draw_screen_quad();

    context.bind_FrameBuffer(FB_Bloom_Main);
    context.bind_Texture(TEX_Bloom_Blur, 0u);
    context.bind_Program(PROG_Bloom_BlurV);
    sq::draw_screen_quad();

    context.bind_FrameBuffer(FB_Bloom_Blur);
    context.bind_Texture(textures.Effects_Bloom, 0u);
    context.bind_Program(PROG_Bloom_BlurH);
    sq::draw_screen_quad();

    context.bind_FrameBuffer(FB_Bloom_Main);
    context.bind_Texture(TEX_Bloom_Blur, 0u);
    context.bind_Program(PROG_Bloom_BlurV);
    sq::draw_screen_quad();
}

//============================================================================//

void EffectsPasses::render_effect_Shafts()
{
    //-- blur the volumetric shafts effect texture -----------//

    if (!options.Shafts_Quality) return;

    //--------------------------------------------------------//

    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Disable);

    context.set_ViewPort(options.Window_Size / 2u);
    context.bind_Texture(textures.Depth_HalfSize, 1u);

    //--------------------------------------------------------//

    context.bind_FrameBuffer(FB_Shafts_Blur);
    context.bind_Texture(textures.Volumetric_Shafts, 0u);
    context.bind_Program(PROG_Shafts_BlurH);
    sq::draw_screen_quad();

    context.bind_FrameBuffer(FB_Shafts_Main);
    context.bind_Texture(TEX_Shafts_Blur, 0u);
    context.bind_Program(PROG_Shafts_BlurV);
    sq::draw_screen_quad();

    context.bind_FrameBuffer(FB_Shafts_Blur);
    context.bind_Texture(textures.Volumetric_Shafts, 0u);
    context.bind_Program(PROG_Shafts_BlurH);
    sq::draw_screen_quad();

    context.bind_FrameBuffer(FB_Shafts_Main);
    context.bind_Texture(TEX_Shafts_Blur, 0u);
    context.bind_Program(PROG_Shafts_BlurV);
    sq::draw_screen_quad();
}

//============================================================================//

void EffectsPasses::render_effect_Overlay()
{
    //-- render the vignette overlay -------------------------//

    if (!options.Vignette_Enable) return;

    //--------------------------------------------------------//

    context.bind_FrameBuffer_default();

    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Alpha);

    context.bind_Program(PROG_Overlay);

    sq::draw_screen_quad();
}
