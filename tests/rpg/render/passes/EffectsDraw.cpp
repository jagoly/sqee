#include <sqee/gl/Context.hpp>
#include <sqee/gl/Drawing.hpp>

#include "../../Options.hpp"
#include "EffectsDraw.hpp"

using Context = sq::Context;
using namespace sqt::render;


EffectsPasses::EffectsPasses(const SharedStuff& _stuff) : SharedStuff(_stuff) {

    FB_SSAO_Main.draw_buffers({gl::COLOR_ATTACHMENT0});
    FB_SSAO_Blur.draw_buffers({gl::COLOR_ATTACHMENT0});
    FB_Bloom_Main.draw_buffers({gl::COLOR_ATTACHMENT0});
    FB_Bloom_Blur.draw_buffers({gl::COLOR_ATTACHMENT0});
    FB_Shafts_Main.draw_buffers({gl::COLOR_ATTACHMENT0});
    FB_Shafts_Blur.draw_buffers({gl::COLOR_ATTACHMENT0});
}


void EffectsPasses::update_options() {

    static const auto& options = Options::get();

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
    else TEX_SSAO_Blur.allocate_storage(options.Window_Size / 2u, false);

    // delete or allocate bloom blur texture
    if (options.Bloom_Enable == false) TEX_Bloom_Blur.delete_object();
    else TEX_Bloom_Blur.allocate_storage(options.Window_Size / 4u, false);

    // delete or allocate shafts blur texture
    if (options.Shafts_Quality == 0u) TEX_Shafts_Blur.delete_object();
    else TEX_Shafts_Blur.allocate_storage(options.Window_Size / 2u, false);

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


void EffectsPasses::render_effect_SSAO() {

    static auto& context = Context::get();
    static const auto& options = Options::get();

    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Disable);

    if (options.SSAO_Quality != 0) {
        context.set_ViewPort(options.Window_Size / 2u);
        shaders.pipeline.use_shader(shaders.VS_FullScreen);

        context.bind_FrameBuffer(FB_SSAO_Main);
        context.bind_Texture(textures.Depth_HalfSize, 1u);
        shaders.pipeline.use_shader(FS_SSAO_Main);
        sq::draw_screen_quad();

        context.bind_FrameBuffer(FB_SSAO_Blur);
        context.bind_Texture(textures.Effects_SSAO, 0u);
        shaders.pipeline.use_shader(FS_SSAO_Blur);
        sq::draw_screen_quad();

        context.bind_FrameBuffer(FB_SSAO_Main);
        context.bind_Texture(TEX_SSAO_Blur, 0u);
        shaders.pipeline.use_shader(FS_SSAO_Blur);
        sq::draw_screen_quad();
    }
}


void EffectsPasses::render_effect_Bloom() {

    static auto& context = Context::get();
    static const auto& options = Options::get();

    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Disable);

    if (options.Bloom_Enable == true) {
        context.set_ViewPort(options.Window_Size / 4u);
        shaders.pipeline.use_shader(shaders.VS_FullScreen);

        context.bind_FrameBuffer(FB_Bloom_Main);
        context.bind_Texture(textures.Lighting_Main, 0u);
        shaders.pipeline.use_shader(FS_Bloom_Main);
        sq::draw_screen_quad();

        context.bind_FrameBuffer(FB_Bloom_Blur);
        context.bind_Texture(textures.Effects_Bloom, 0u);
        shaders.pipeline.use_shader(FS_Bloom_BlurH);
        sq::draw_screen_quad();

        context.bind_FrameBuffer(FB_Bloom_Main);
        context.bind_Texture(TEX_Bloom_Blur, 0u);
        shaders.pipeline.use_shader(FS_Bloom_BlurV);
        sq::draw_screen_quad();

        context.bind_FrameBuffer(FB_Bloom_Blur);
        context.bind_Texture(textures.Effects_Bloom, 0u);
        shaders.pipeline.use_shader(FS_Bloom_BlurH);
        sq::draw_screen_quad();

        context.bind_FrameBuffer(FB_Bloom_Main);
        context.bind_Texture(TEX_Bloom_Blur, 0u);
        shaders.pipeline.use_shader(FS_Bloom_BlurV);
        sq::draw_screen_quad();
    }
}


void EffectsPasses::render_effect_Shafts() {

    static auto& context = Context::get();
    static const auto& options = Options::get();

    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Disable);

    if (options.Shafts_Quality != 0u) {
        context.set_ViewPort(options.Window_Size / 2u);
        shaders.pipeline.use_shader(shaders.VS_FullScreen);
        context.bind_Texture(textures.Depth_HalfSize, 1u);

        context.bind_FrameBuffer(FB_Shafts_Blur);
        context.bind_Texture(textures.Volumetric_Shafts, 0u);
        shaders.pipeline.use_shader(FS_Shafts_BlurH);
        sq::draw_screen_quad();

        context.bind_FrameBuffer(FB_Shafts_Main);
        context.bind_Texture(TEX_Shafts_Blur, 0u);
        shaders.pipeline.use_shader(FS_Shafts_BlurV);
        sq::draw_screen_quad();

        context.bind_FrameBuffer(FB_Shafts_Blur);
        context.bind_Texture(textures.Volumetric_Shafts, 0u);
        shaders.pipeline.use_shader(FS_Shafts_BlurH);
        sq::draw_screen_quad();

        context.bind_FrameBuffer(FB_Shafts_Main);
        context.bind_Texture(TEX_Shafts_Blur, 0u);
        shaders.pipeline.use_shader(FS_Shafts_BlurV);
        sq::draw_screen_quad();
    }
}


void EffectsPasses::render_effect_Overlay() {

    static auto& context = Context::get();
    static const auto& options = Options::get();

    gl::BindFramebuffer(gl::FRAMEBUFFER, 0u);

    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Alpha);

    // apply vignette effect if enabled
    if (options.Vignette_Enable == true) {
        shaders.pipeline.use_shader(FS_Overlay);
        sq::draw_screen_quad();
    }
}
