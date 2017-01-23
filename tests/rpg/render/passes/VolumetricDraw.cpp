#include <sqee/gl/Context.hpp>
#include <sqee/gl/Drawing.hpp>

#include "../../Options.hpp"
#include "VolumetricDraw.hpp"

using Context = sq::Context;
using namespace sqt::render;

//============================================================================//

VolumetricPasses::VolumetricPasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    FB_Shafts.draw_buffers({gl::COLOR_ATTACHMENT0});

    VS_Shafts_Stencil.add_uniform("matrix"); // Mat4F

    shaders.preprocs(VS_Shafts_Stencil, "volumetric/Shafts/Stencil_vs");
    shaders.preprocs(VS_Shafts_Light, "volumetric/Shafts/Light_vs");
}

//============================================================================//

void VolumetricPasses::update_options()
{
    if (options.Shafts_Quality) shaders.preprocs(FS_Shafts_LightCasc, "volumetric/Shafts/LightCasc_fs");
    if (options.Shafts_Quality) shaders.preprocs(FS_Shafts_LightOrtho, "volumetric/Shafts/LightOrtho_fs");
    if (options.Shafts_Quality) shaders.preprocs(FS_Shafts_LightPoint, "volumetric/Shafts/LightPoint_fs");
    if (options.Shafts_Quality) shaders.preprocs(FS_Shafts_LightSpot, "volumetric/Shafts/LightSpot_fs");

    // delete or allocate shafts depth stencil texture
    if (options.Shafts_Quality == 0u) TEX_Shafts_Depth.delete_object();
    else TEX_Shafts_Depth.allocate_storage(options.Window_Size / 2u);

    // re-attach textures to shafts framebuffer
    if (options.Shafts_Quality) FB_Shafts.attach(gl::COLOR_ATTACHMENT0, textures.Volumetric_Shafts);
    if (options.Shafts_Quality) FB_Shafts.attach(gl::DEPTH_STENCIL_ATTACHMENT, TEX_Shafts_Depth);

    FB_HalfDepthBlit.attach(gl::DEPTH_STENCIL_ATTACHMENT, textures.Depth_HalfSize);
}

//============================================================================//

void VolumetricPasses::render(const data::VolumetricPasses& data)
{
    if (options.Shafts_Quality == 0u) return;

    // bind and clear framebuffer
    context.set_ViewPort(options.Window_Size / 2u);
    context.bind_FrameBuffer(FB_Shafts);
    context.clear_Colour(Vec4F(0.f));

    // set blending mode for light accumulation
    context.set_state(Context::Blend_Mode::Accumulate);


    context.bind_Texture(TEX_Shafts_Depth, 7u);

    // render cascade light shafts pass without stencil
    if (data.skylightPass != nullptr) impl_render_SkyLightPass(*data.skylightPass);

    context.bind_Texture(TEX_Shafts_Depth, 7u);

    // render shafts of all other types of lights using stencil volumes
//    for (const auto& light : _data.orthoPassVec) impl_render_StencilPass(light, volumes.Light_Ortho, FS_Shafts_LightOrtho);
    for (const auto& light : data.pointPassVec) impl_render_StencilPass(light, volumes.Light_Point, FS_Shafts_LightPoint);
    for (const auto& light : data.spotPassVec) impl_render_StencilPass(light, volumes.Light_Spot, FS_Shafts_LightSpot);
}

//============================================================================//

void VolumetricPasses::impl_render_SkyLightPass(const data::VolumetricSkyLightPass& light)
{
    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);

    context.bind_UniformBuffer(light.ubo, 1u);
    context.bind_Texture(textures.Depth_HalfSize, 7u);
    context.bind_Texture(light.tex, 8u);

    context.use_Shader_Vert(VS_Shafts_Light);
    context.use_Shader_Frag(FS_Shafts_LightCasc);
    sq::draw_screen_quad();
}

//============================================================================//

void VolumetricPasses::impl_render_StencilPass(const data::VolumetricStencilPass& light,
                                               const sq::Volume& volume, const sq::Shader& shader)
{
    FB_HalfDepthBlit.blit(FB_Shafts, options.Window_Size / 2u, gl::DEPTH_BUFFER_BIT);


    context.use_Shader_Vert(VS_Shafts_Stencil);
    context.disable_shader_stage_fragment();

    VS_Shafts_Stencil.update<Mat4F>("matrix", light.matrix);


    if (options.Vignette_Enable && light.stencil == true)
    {
        context.set_state(Context::Cull_Face::Back);
        context.set_state(Context::Depth_Test::Keep);
        context.set_state(Context::Stencil_Test::Replace);
        context.set_Stencil_Params(gl::ALWAYS, 2, 0, 2);

        context.clear_Stencil(0, 255);

        volume.bind_and_draw(context);

        context.set_state(Context::Stencil_Test::Keep);
        context.set_Stencil_Params(gl::EQUAL, 2, 2, 0);
    }
    else context.set_state(Context::Stencil_Test::Disable);


    context.set_state(Context::Cull_Face::Front);
    context.set_state(Context::Depth_Test::Replace);

    volume.bind_and_draw(context);


    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);

    context.bind_UniformBuffer(light.ubo, 1u);
    context.bind_Texture(light.tex, 8u);

    context.use_Shader_Vert(VS_Shafts_Light);
    context.use_Shader_Frag(shader);
    sq::draw_screen_quad();
}

//============================================================================//
