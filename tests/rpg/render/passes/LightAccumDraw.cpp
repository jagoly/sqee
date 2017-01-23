#include <sqee/gl/Context.hpp>
#include <sqee/gl/Drawing.hpp>

#include "../../Options.hpp"
#include "LightAccumDraw.hpp"

using Context = sq::Context;
using namespace sqt::render;

//============================================================================//

LightAccumPasses::LightAccumPasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    FB_Lighting.draw_buffers({gl::COLOR_ATTACHMENT0});

    VS_Main_Stencil.add_uniform("matrix"); // Mat4F

    shaders.preprocs(VS_Main_Stencil, "lighting/Main/Stencil_vs");
}

//============================================================================//

void LightAccumPasses::update_options()
{
    shaders.preprocs(FS_Main_LightOrtho_base, "lighting/Main/LightOrtho_fs");
    shaders.preprocs(FS_Main_LightPoint_base, "lighting/Main/LightPoint_fs");
    shaders.preprocs(FS_Main_LightSpot_base, "lighting/Main/LightSpot_fs");

    shaders.preprocs(FS_Main_LightCasc_shad, "lighting/Main/LightCasc_fs", "#define SHADOW");
    shaders.preprocs(FS_Main_LightOrtho_shad, "lighting/Main/LightOrtho_fs", "#define SHADOW");
    shaders.preprocs(FS_Main_LightPoint_shad, "lighting/Main/LightPoint_fs", "#define SHADOW");
    shaders.preprocs(FS_Main_LightSpot_shad, "lighting/Main/LightSpot_fs", "#define SHADOW");

    // re-attach textures to lighting framebuffer
    FB_Lighting.attach(gl::COLOR_ATTACHMENT0, textures.Lighting_Main);
    FB_Lighting.attach(gl::DEPTH_STENCIL_ATTACHMENT, textures.Depth_FullSize);
}

//============================================================================//

void LightAccumPasses::render(const data::LightAccumPasses& data)
{
    // bind framebuffer, but don't clear
    context.set_ViewPort(options.Window_Size);
    context.bind_FrameBuffer(FB_Lighting);

    // face culling is not used at all
    context.set_state(Context::Cull_Face::Disable);

    // set blending mode for light accumulation
    context.set_state(Context::Blend_Mode::Accumulate);

    // bind all gbuffer textures to their slots
    context.bind_Texture(textures.Gbuffer_MainDiff, 3u);
    context.bind_Texture(textures.Gbuffer_MainSurf, 4u);
    context.bind_Texture(textures.Gbuffer_MainNorm, 5u);
    context.bind_Texture(textures.Gbuffer_MainSpec, 6u);
    context.bind_Texture(textures.Depth_FullSize, 7u);

    // render cascade light pass without stencil
    if (data.skylightPass != nullptr) impl_render_SkyLightPass(*data.skylightPass);

    // render all other types of lights using stencil volumes
    for (const auto& light : data.orthoPassVec) impl_render_StencilPass(light, volumes.Light_Ortho, FS_Main_LightOrtho_shad);
    for (const auto& light : data.pointPassVec) impl_render_StencilPass(light, volumes.Light_Point, FS_Main_LightPoint_shad);
    for (const auto& light : data.spotPassVec) impl_render_StencilPass(light, volumes.Light_Spot, FS_Main_LightSpot_shad);
}

//============================================================================//

void LightAccumPasses::impl_render_SkyLightPass(const data::LightAccumSkyLightPass& light)
{
    // disable depth test for light accumulation
    context.set_state(Context::Depth_Test::Disable);

    // only render where there is geometry
    context.set_state(Context::Stencil_Test::Keep);
    context.set_Stencil_Params(gl::EQUAL, 1, 1, 0);

    context.use_Shader_Vert(shaders.VS_FullScreen);
    context.use_Shader_Frag(FS_Main_LightCasc_shad);

    context.bind_UniformBuffer(light.ubo, 1u);
    context.bind_Texture(light.tex, 8u);

    // render full screen lighting quad
    sq::draw_screen_quad();
}

//============================================================================//

void LightAccumPasses::impl_render_StencilPass(const data::LightAccumStencilPass& light,
                                               const sq::Volume& volume, const sq::Shader& shader)
{
    // enable depth test for stenciling
    context.set_state(Context::Depth_Test::Keep);

    // use two-sided light volume stencil trick
    context.set_state(Context::Stencil_Test::Custom);
    context.set_Stencil_Custom(gl::KEEP, gl::INVERT, gl::KEEP);

    // only render where there is geometry
    context.set_Stencil_Params(gl::EQUAL, 1, 1, 2);

    // clear light volume stencil bit
    context.clear_Stencil(0, 2);

    context.use_Shader_Vert(VS_Main_Stencil);
    context.disable_shader_stage_fragment();

    VS_Main_Stencil.update<Mat4F>("matrix", light.matrix);

    volume.bind_and_draw(context);

    context.use_Shader_Vert(shaders.VS_FullScreen);
    context.use_Shader_Frag(shader);

    // disable depth test for light accumulation
    context.set_state(Context::Depth_Test::Disable);

    // only render over the stencil volume bit
    context.set_state(Context::Stencil_Test::Keep);
    context.set_Stencil_Params(gl::EQUAL, 2, 2, 0);

    context.bind_UniformBuffer(light.ubo, 1u);
    context.bind_Texture(light.tex, 8u);

    // render full screen lighting quad
    sq::draw_screen_quad();
}

//============================================================================//
