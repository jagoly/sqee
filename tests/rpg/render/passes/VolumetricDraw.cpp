#include <sqee/gl/Context.hpp>
#include <sqee/gl/Drawing.hpp>

#include <sqee/redist/gl_loader.hpp>

#include "VolumetricDraw.hpp"

using Context = sq::Context;
using namespace sqt::render;

//============================================================================//

VolumetricPasses::VolumetricPasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    FB_Shafts.draw_buffers({gl::COLOR_ATTACHMENT0});

    processor.load_vertex(PROG_Shafts_Stencil, "volumetric/Shafts/Stencil_vs");
    processor.load_vertex(PROG_Shafts_LightCasc, "volumetric/Shafts/Light_vs");
    processor.load_vertex(PROG_Shafts_LightOrtho, "volumetric/Shafts/Light_vs");
    processor.load_vertex(PROG_Shafts_LightPoint, "volumetric/Shafts/Light_vs");
    processor.load_vertex(PROG_Shafts_LightSpot, "volumetric/Shafts/Light_vs");

    PROG_Shafts_Stencil.link_program_stages();
}

//============================================================================//

void VolumetricPasses::update_options()
{
    if (options.Shafts_Quality)
    {
        processor.load_fragment(PROG_Shafts_LightCasc, "volumetric/Shafts/LightCasc_fs");
        processor.load_fragment(PROG_Shafts_LightOrtho, "volumetric/Shafts/LightOrtho_fs");
        processor.load_fragment(PROG_Shafts_LightPoint, "volumetric/Shafts/LightPoint_fs");
        processor.load_fragment(PROG_Shafts_LightSpot, "volumetric/Shafts/LightSpot_fs");

        PROG_Shafts_LightCasc.link_program_stages();
        PROG_Shafts_LightOrtho.link_program_stages();
        PROG_Shafts_LightPoint.link_program_stages();
        PROG_Shafts_LightSpot.link_program_stages();
    }

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

    //--------------------------------------------------------//

    // bind and clear framebuffer
    context.set_ViewPort(options.Window_Size / 2u);
    context.bind_FrameBuffer(FB_Shafts);
    context.clear_Colour(Vec4F(0.f));

    context.set_state(Context::Blend_Mode::Accumulate);

    //--------------------------------------------------------//

    // render cascade light shafts pass without stencil
    if (data.skylightPass != nullptr) impl_render_SkyLightPass(*data.skylightPass);

    // render shafts of all other types of lights using stencil volumes
    for (auto& light : data.orthoPassVec) impl_render_StencilPass(light, volumes.Light_Ortho, PROG_Shafts_LightOrtho);
    for (auto& light : data.pointPassVec) impl_render_StencilPass(light, volumes.Light_Point, PROG_Shafts_LightPoint);
    for (auto& light : data.spotPassVec) impl_render_StencilPass(light, volumes.Light_Spot, PROG_Shafts_LightSpot);
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

    context.bind_Program(PROG_Shafts_LightCasc);

    sq::draw_screen_quad();
}

//============================================================================//

void VolumetricPasses::impl_render_StencilPass(const data::VolumetricStencilPass& light,
                                               const sq::Volume& volume, const sq::Program& program)
{
    FB_HalfDepthBlit.blit(FB_Shafts, options.Window_Size / 2u, gl::DEPTH_BUFFER_BIT);

    context.bind_Program(PROG_Shafts_Stencil);

    PROG_Shafts_Stencil.update(0, light.matrix);

    //--------------------------------------------------------//

    if (light.stencil == true)
    {
        context.clear_Stencil(0, 255);

        context.set_state(Context::Depth_Clamp::Enable);

        context.set_state(Context::Cull_Face::Back);
        context.set_state(Context::Depth_Test::Keep);
        context.set_state(Context::Stencil_Test::Replace);
        context.set_Stencil_Params({gl::ALWAYS, 2, 0, 2});

        volume.bind_and_draw(context);

        context.set_state(Context::Depth_Clamp::Disable);

        context.set_state(Context::Stencil_Test::Keep);
        context.set_Stencil_Params({gl::EQUAL, 2, 2, 0});
    }
    else context.set_state(Context::Stencil_Test::Disable);

    //--------------------------------------------------------//

    context.set_state(Context::Cull_Face::Front);
    context.set_state(Context::Depth_Test::Replace);

    volume.bind_and_draw(context);

    //--------------------------------------------------------//

    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);

    context.bind_UniformBuffer(light.ubo, 1u);

    context.bind_Texture(TEX_Shafts_Depth, 7u);
    context.bind_Texture(light.tex, 8u);

    context.bind_Program(program);

    sq::draw_screen_quad();
}
