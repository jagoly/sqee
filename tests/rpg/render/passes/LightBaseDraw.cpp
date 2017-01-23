#include <sqee/gl/Context.hpp>
#include <sqee/gl/Drawing.hpp>

#include "../../Options.hpp"
#include "LightBaseDraw.hpp"

using Context = sq::Context;
using namespace sqt::render;

//============================================================================//

LightBasePasses::LightBasePasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    FB_Lighting.draw_buffers({gl::COLOR_ATTACHMENT0});

    FS_Main_Ambient.add_uniform("colour"); // Vec3F
    FS_Skybox.add_uniform("params"); // Vec4F

    shaders.preprocs(VS_Main_Skybox, "lighting/Main/Skybox_vs");
    shaders.preprocs(FS_Skybox, "lighting/Skybox_fs");
}

//============================================================================//

void LightBasePasses::update_options()
{
    shaders.preprocs(FS_Main_Ambient, "lighting/Main/Ambient_fs");

    // re-attach textures to lighting framebuffer
    FB_Lighting.attach(gl::COLOR_ATTACHMENT0, textures.Lighting_Main);
    FB_Lighting.attach(gl::DEPTH_STENCIL_ATTACHMENT, textures.Depth_FullSize);
}

//============================================================================//

void LightBasePasses::render(const data::LightBasePasses& data)
{
    // bind and clear framebuffer
    context.set_ViewPort(options.Window_Size);
    context.bind_FrameBuffer(FB_Lighting);
    context.clear_Colour(Vec4F(0.f));

    // stencil testing for geometry is used
    context.set_state(Context::Stencil_Test::Keep);

    // disable cull face, depth test and blend
    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Blend_Mode::Disable);

    // bind all gbuffer textures to their slots
    context.bind_Texture(textures.Gbuffer_MainDiff, 3u);
    context.bind_Texture(textures.Gbuffer_MainSurf, 4u);
    context.bind_Texture(textures.Gbuffer_MainNorm, 5u);
    context.bind_Texture(textures.Gbuffer_MainSpec, 6u);
    context.bind_Texture(textures.Depth_FullSize, 7u);

    ////// sub passes //////
    if (data.skyboxPass != nullptr) impl_render_SkyBoxPass(*data.skyboxPass);
    if (data.ambientPass != nullptr) impl_render_AmbientPass(*data.ambientPass);
}

//============================================================================//

void LightBasePasses::impl_render_SkyBoxPass(const data::LightBaseSkyBoxPass& data)
{
    // only render where there is no geometry
    context.set_Stencil_Params(gl::EQUAL, 0, 1, 0);

    context.use_Shader_Vert(VS_Main_Skybox);
    context.use_Shader_Frag(FS_Skybox);

    // enable alpha blending
    //context.set_state(Context::Blend_Mode::Alpha);

    FS_Skybox.update<Vec4F>("params", data.params);
    context.bind_Texture(data.tex, 0u);
    sq::draw_screen_quad();
}

//============================================================================//

void LightBasePasses::impl_render_AmbientPass(const data::LightBaseAmbientPass& data)
{
    // only render where there is geometry
    context.set_Stencil_Params(gl::EQUAL, 1, 1, 0);

    context.use_Shader_Vert(shaders.VS_FullScreen);
    context.use_Shader_Frag(FS_Main_Ambient);

    if (options.SSAO_Quality != 0u)
    {
        // bind textures required for SSAO
        context.bind_Texture(textures.Effects_SSAO, 0u);
        context.bind_Texture(textures.Depth_HalfSize, 1u);
    }

    FS_Main_Ambient.update<Vec3F>("colour", data.colour);
    sq::draw_screen_quad();
}

//============================================================================//
