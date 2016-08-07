#include <sqee/gl/Context.hpp>
#include <sqee/gl/Drawing.hpp>

#include "../../Options.hpp"
#include "CompositeDraw.hpp"

using Context = sq::Context;
using namespace sqt::render;


CompositePasses::CompositePasses(const SharedStuff& _stuff) : SharedStuff(_stuff) {

}


void CompositePasses::update_options() {

    static const auto& options = Options::get();

    shaders.preprocs(FS_Composite, "composite/Composite_fs");

    if (options.FSAA_Quality) shaders.preprocs(FS_FSAA_Screen, "composite/FSAA/FXAA_fs");

    // re-attach texture to composite framebuffer
    FB_Composite.attach(gl::COLOR_ATTACHMENT0, textures.Composite_Main);
}


void CompositePasses::render_to_screen() {

    static auto& context = Context::get();
    static const auto& options = Options::get();

    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Disable);

    context.set_ViewPort(options.Window_Size);

    context.bind_Texture(textures.Lighting_Main, 0u);
    shaders.pipeline.use_shader(shaders.VS_FullScreen);

    if (options.Shafts_Quality != 0u) {
        // bind textures required for light shafts
        context.bind_Texture(textures.Volumetric_Shafts, 1u);
        context.bind_Texture(textures.Depth_HalfSize, 2u);
        context.bind_Texture(textures.Depth_FullSize, 3u);
    }

    if (options.Bloom_Enable == true) {
        // bind texture required for bloom effect
        context.bind_Texture(textures.Effects_Bloom, 4u);
    }

    shaders.pipeline.use_shader(FS_Composite);
    context.bind_FrameBuffer(FB_Composite);
    sq::draw_screen_quad();

    gl::BindFramebuffer(gl::FRAMEBUFFER, 0u);
    context.bind_Texture(textures.Composite_Main, 0u);

    // use full-screen anti-aliasing shader if enabled
    if (options.FSAA_Quality > 0) shaders.pipeline.use_shader(FS_FSAA_Screen);
    else shaders.pipeline.use_shader(shaders.FS_PassThrough);

    if (options.Debug_Texture.empty() == false) {
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
}
