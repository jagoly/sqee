#include <sqee/gl/Context.hpp>
#include <sqee/gl/Drawing.hpp>

#include "../../Options.hpp"
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
}

//============================================================================//

void EffectsPasses::update_options()
{
    if (options.SSAO_Quality) shaders.preprocs(FS_SSAO_Main, "effects/SSAO/Main_fs");
    if (options.SSAO_Quality) shaders.preprocs(FS_SSAO_Blur, "effects/SSAO/Blur_fs");

    if (options.Bloom_Enable) shaders.preprocs(FS_Bloom_Main, "effects/Bloom/Main_fs");
    if (options.Bloom_Enable) shaders.preprocs(FS_Bloom_BlurH, "effects/Bloom/BlurH_fs");
    if (options.Bloom_Enable) shaders.preprocs(FS_Bloom_BlurV, "effects/Bloom/BlurV_fs");

    if (options.Shafts_Quality) shaders.preprocs(FS_Shafts_BlurH, "effects/Shafts/BlurH_fs");
    if (options.Shafts_Quality) shaders.preprocs(FS_Shafts_BlurV, "effects/Shafts/BlurV_fs");

    shaders.preprocs(FS_Overlay, "effects/Overlay_fs");

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
    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Disable);

    if (options.SSAO_Quality != 0)
    {
        context.set_ViewPort(options.Window_Size / 2u);
        context.use_Shader_Vert(shaders.VS_FullScreen);

        context.bind_FrameBuffer(FB_SSAO_Main);
        context.bind_Texture(textures.Depth_HalfSize, 1u);
        context.use_Shader_Frag(FS_SSAO_Main);
        sq::draw_screen_quad();

        context.bind_FrameBuffer(FB_SSAO_Blur);
        context.bind_Texture(textures.Effects_SSAO, 0u);
        context.use_Shader_Frag(FS_SSAO_Blur);
        sq::draw_screen_quad();

        context.bind_FrameBuffer(FB_SSAO_Main);
        context.bind_Texture(TEX_SSAO_Blur, 0u);
        context.use_Shader_Frag(FS_SSAO_Blur);
        sq::draw_screen_quad();
    }
}

//============================================================================//

void EffectsPasses::render_effect_Bloom()
{
    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Disable);

    if (options.Bloom_Enable == true)
    {
        context.set_ViewPort(options.Window_Size / 4u);
        context.use_Shader_Vert(shaders.VS_FullScreen);

        context.bind_FrameBuffer(FB_Bloom_Main);
        context.bind_Texture(textures.Lighting_Main, 0u);
        context.use_Shader_Frag(FS_Bloom_Main);
        sq::draw_screen_quad();

        context.bind_FrameBuffer(FB_Bloom_Blur);
        context.bind_Texture(textures.Effects_Bloom, 0u);
        context.use_Shader_Frag(FS_Bloom_BlurH);
        sq::draw_screen_quad();

        context.bind_FrameBuffer(FB_Bloom_Main);
        context.bind_Texture(TEX_Bloom_Blur, 0u);
        context.use_Shader_Frag(FS_Bloom_BlurV);
        sq::draw_screen_quad();

        context.bind_FrameBuffer(FB_Bloom_Blur);
        context.bind_Texture(textures.Effects_Bloom, 0u);
        context.use_Shader_Frag(FS_Bloom_BlurH);
        sq::draw_screen_quad();

        context.bind_FrameBuffer(FB_Bloom_Main);
        context.bind_Texture(TEX_Bloom_Blur, 0u);
        context.use_Shader_Frag(FS_Bloom_BlurV);
        sq::draw_screen_quad();
    }
}

//============================================================================//

void EffectsPasses::render_effect_Shafts()
{
    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Disable);

    if (options.Shafts_Quality != 0u)
    {
        context.set_ViewPort(options.Window_Size / 2u);
        context.use_Shader_Vert(shaders.VS_FullScreen);
        context.bind_Texture(textures.Depth_HalfSize, 1u);

        context.bind_FrameBuffer(FB_Shafts_Blur);
        context.bind_Texture(textures.Volumetric_Shafts, 0u);
        context.use_Shader_Frag(FS_Shafts_BlurH);
        sq::draw_screen_quad();

        context.bind_FrameBuffer(FB_Shafts_Main);
        context.bind_Texture(TEX_Shafts_Blur, 0u);
        context.use_Shader_Frag(FS_Shafts_BlurV);
        sq::draw_screen_quad();

        context.bind_FrameBuffer(FB_Shafts_Blur);
        context.bind_Texture(textures.Volumetric_Shafts, 0u);
        context.use_Shader_Frag(FS_Shafts_BlurH);
        sq::draw_screen_quad();

        context.bind_FrameBuffer(FB_Shafts_Main);
        context.bind_Texture(TEX_Shafts_Blur, 0u);
        context.use_Shader_Frag(FS_Shafts_BlurV);
        sq::draw_screen_quad();
    }
}

//============================================================================//

void EffectsPasses::render_effect_Overlay()
{
    context.bind_FrameBuffer_default();

    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Alpha);

    // apply vignette effect if enabled
    if (options.Vignette_Enable == true)
    {
        context.use_Shader_Frag(FS_Overlay);
        sq::draw_screen_quad();
    }
}

//============================================================================//
