#include <sqee/gl/Context.hpp>

#include "VolumetricDraw.hpp"

using namespace sqt::render;

//============================================================================//

VolumetricPasses::VolumetricPasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    FB_Shafts.draw_buffers({sq::FboAttach::Colour0});

    processor.load_vertex(PROG_Shafts_Stencil, "shaders/volumetric/Shafts/Stencil_vs.glsl", {});
    processor.load_vertex(PROG_Shafts_LightCasc, "shaders/volumetric/Shafts/Light_vs.glsl", {});
    processor.load_vertex(PROG_Shafts_LightOrtho, "shaders/volumetric/Shafts/Light_vs.glsl", {});
    processor.load_vertex(PROG_Shafts_LightPoint, "shaders/volumetric/Shafts/Light_vs.glsl", {});
    processor.load_vertex(PROG_Shafts_LightSpot, "shaders/volumetric/Shafts/Light_vs.glsl", {});

    PROG_Shafts_Stencil.link_program_stages();
}

//============================================================================//

void VolumetricPasses::update_options()
{
    TEX_Shafts_Depth = sq::Texture2D();

    if (options.Shafts_Quality > 0u)
    {
        processor.load_fragment(PROG_Shafts_LightCasc, "shaders/volumetric/Shafts/LightCasc_fs.glsl", {});
        processor.load_fragment(PROG_Shafts_LightOrtho, "shaders/volumetric/Shafts/LightOrtho_fs.glsl", {});
        processor.load_fragment(PROG_Shafts_LightPoint, "shaders/volumetric/Shafts/LightPoint_fs.glsl", {});
        processor.load_fragment(PROG_Shafts_LightSpot, "shaders/volumetric/Shafts/LightSpot_fs.glsl", {});

        PROG_Shafts_LightCasc.link_program_stages();
        PROG_Shafts_LightOrtho.link_program_stages();
        PROG_Shafts_LightPoint.link_program_stages();
        PROG_Shafts_LightSpot.link_program_stages();

        TEX_Shafts_Depth.allocate_storage(sq::TexFormat::DEP24S8, options.Window_Size / 2u, false);
        FB_Shafts.attach(sq::FboAttach::Colour0, textures.Volumetric_Shafts);
        FB_Shafts.attach(sq::FboAttach::DepthStencil, TEX_Shafts_Depth);

        FB_HalfDepthBlit.attach(sq::FboAttach::DepthStencil, textures.Depth_HalfSize);
    }
}

//============================================================================//

void VolumetricPasses::render(const data::VolumetricPasses& data)
{
    if (options.Shafts_Quality == 0u) return;

    //--------------------------------------------------------//

    // bind and clear framebuffer
    context.set_ViewPort(options.Window_Size / 2u);
    context.bind_framebuffer(FB_Shafts);
    context.clear_colour(Vec4F(0.f));

    context.set_state(sq::BlendMode::Accumulate);

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
    context.set_state(sq::CullFace::Disable);
    context.set_state(sq::DepthTest::Disable);
    context.set_state(sq::StencilTest::Disable);

    context.bind_buffer(light.ubo, sq::BufTarget::Uniform, 1u);

    context.bind_texture(textures.Depth_HalfSize, 7u);
    context.bind_texture(light.tex, 8u);

    context.bind_program(PROG_Shafts_LightCasc);

    context.bind_vertexarray_dummy();
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);
}

//============================================================================//

void VolumetricPasses::impl_render_StencilPass(const data::VolumetricStencilPass& light,
                                               const sq::Volume& volume, const sq::Program& program)
{
    FB_HalfDepthBlit.blit(FB_Shafts, options.Window_Size / 2u, sq::BlitMask::Depth);

    context.bind_program(PROG_Shafts_Stencil);

    PROG_Shafts_Stencil.update(0, light.matrix);

    //--------------------------------------------------------//

    volume.apply_to_context(context);

    if (light.stencil == true)
    {
        context.clear_stencil(0x00, 0xFF);

        context.set_state(sq::DepthClamp::Enable);

        context.set_state(sq::CullFace::Back);
        context.set_state(sq::DepthTest::Keep);
        context.set_state(sq::StencilTest::Replace);
        context.set_stencil_params(sq::CompareFunc::Always, 0x02, 0x00, 0x02);

        volume.draw(context);

        context.set_state(sq::DepthClamp::Disable);

        context.set_state(sq::StencilTest::Keep);
        context.set_stencil_params(sq::CompareFunc::Always, 0x02, 0x02, 0x00);
    }
    else context.set_state(sq::StencilTest::Disable);

    context.set_state(sq::CullFace::Front);
    context.set_state(sq::DepthTest::Replace);

    volume.draw(context);

    //--------------------------------------------------------//

    context.set_state(sq::CullFace::Disable);
    context.set_state(sq::DepthTest::Disable);

    context.bind_buffer(light.ubo, sq::BufTarget::Uniform, 1u);

    context.bind_texture(TEX_Shafts_Depth, 7u);
    context.bind_texture(light.tex, 8u);

    context.bind_program(program);

    context.bind_vertexarray_dummy();
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);
}
