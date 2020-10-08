#include <sqee/gl/Context.hpp>

#include "LightAccumDraw.hpp"

using namespace sqt::render;

//============================================================================//

LightAccumPasses::LightAccumPasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    FB_Lighting.draw_buffers({sq::FboAttach::Colour0});

    processor.load_vertex(PROG_Main_Stencil, "shaders/lighting/Main/Stencil_vs.glsl", {});

    processor.load_vertex(PROG_Main_LightCasc, "shaders/generic/FullScreen_vs.glsl", {});
    processor.load_vertex(PROG_Main_LightOrtho_base, "shaders/generic/FullScreen_vs.glsl", {});
    processor.load_vertex(PROG_Main_LightOrtho_shad, "shaders/generic/FullScreen_vs.glsl", {});
    processor.load_vertex(PROG_Main_LightPoint_base, "shaders/generic/FullScreen_vs.glsl", {});
    processor.load_vertex(PROG_Main_LightPoint_shad, "shaders/generic/FullScreen_vs.glsl", {});
    processor.load_vertex(PROG_Main_LightSpot_base, "shaders/generic/FullScreen_vs.glsl", {});
    processor.load_vertex(PROG_Main_LightSpot_shad, "shaders/generic/FullScreen_vs.glsl", {});

    PROG_Main_Stencil.link_program_stages();
}

//============================================================================//

void LightAccumPasses::update_options()
{
    const sq::PreProcessor::OptionMap shadowOptions = { {"SHADOW", "1"} };

    processor.load_fragment(PROG_Main_LightCasc, "shaders/lighting/Main/LightCasc_fs.glsl", {});
    processor.load_fragment(PROG_Main_LightOrtho_base, "shaders/lighting/Main/LightOrtho_fs.glsl", {});
    processor.load_fragment(PROG_Main_LightOrtho_shad, "shaders/lighting/Main/LightOrtho_fs.glsl", shadowOptions);
    processor.load_fragment(PROG_Main_LightPoint_base, "shaders/lighting/Main/LightPoint_fs.glsl", {});
    processor.load_fragment(PROG_Main_LightPoint_shad, "shaders/lighting/Main/LightPoint_fs.glsl", shadowOptions);
    processor.load_fragment(PROG_Main_LightSpot_base, "shaders/lighting/Main/LightSpot_fs.glsl", {});
    processor.load_fragment(PROG_Main_LightSpot_shad, "shaders/lighting/Main/LightSpot_fs.glsl", shadowOptions);

    PROG_Main_LightCasc.link_program_stages();
    PROG_Main_LightOrtho_base.link_program_stages();
    PROG_Main_LightOrtho_shad.link_program_stages();
    PROG_Main_LightPoint_base.link_program_stages();
    PROG_Main_LightPoint_shad.link_program_stages();
    PROG_Main_LightSpot_base.link_program_stages();
    PROG_Main_LightSpot_shad.link_program_stages();

    // re-attach textures to lighting framebuffer
    FB_Lighting.attach(sq::FboAttach::Colour0, textures.Lighting_Main);
    FB_Lighting.attach(sq::FboAttach::DepthStencil, textures.Depth_FullSize);
}

//============================================================================//

void LightAccumPasses::render(const data::LightAccumPasses& data)
{
    // bind framebuffer, but don't clear
    context.set_ViewPort(options.Window_Size);
    context.bind_framebuffer(FB_Lighting);

    // face culling is not used at all
    context.set_state(sq::CullFace::Disable);

    // set blending mode for light accumulation
    context.set_state(sq::BlendMode::Accumulate);

    // bind all gbuffer textures to their slots
    context.bind_texture(textures.Gbuffer_MainDiff, 3u);
    context.bind_texture(textures.Gbuffer_MainSurf, 4u);
    context.bind_texture(textures.Gbuffer_MainNorm, 5u);
    context.bind_texture(textures.Gbuffer_MainSpec, 6u);
    context.bind_texture(textures.Depth_FullSize, 7u);

    //-- render sub passes -----------------------------------//

    // render cascade light pass without stencil
    if (data.skylightPass != nullptr) impl_render_SkyLightPass(*data.skylightPass);

    // render all other types of lights using stencil volumes
    for (auto& light : data.orthoPassVec) impl_render_StencilPass(light, volumes.Light_Ortho, PROG_Main_LightOrtho_shad);
    for (auto& light : data.pointPassVec) impl_render_StencilPass(light, volumes.Light_Point, PROG_Main_LightPoint_shad);
    for (auto& light : data.spotPassVec) impl_render_StencilPass(light, volumes.Light_Spot, PROG_Main_LightSpot_shad);
}

//============================================================================//

void LightAccumPasses::impl_render_SkyLightPass(const data::LightAccumSkyLightPass& light)
{
    // disable depth test for light accumulation
    context.set_state(sq::DepthTest::Disable);

    // only render where there is geometry
    context.set_state(sq::StencilTest::Keep);
    context.set_stencil_params(sq::CompareFunc::Equal, 1, 1, 0);

    context.bind_program(PROG_Main_LightCasc);

    context.bind_buffer(light.ubo, sq::BufTarget::Uniform, 1u);
    context.bind_texture(light.tex, 8u);

    // render full screen lighting quad
    context.bind_vertexarray_dummy();
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);
}

//============================================================================//

void LightAccumPasses::impl_render_StencilPass(const data::LightAccumStencilPass& light,
                                               const sq::Volume& volume, const sq::Program& program)
{
    // enable depth test for stenciling
    context.set_state(sq::DepthTest::Keep);

    // use two-sided light volume stencil trick
    context.set_state(sq::StencilTest::Custom);
    context.set_stencil_custom(sq::StencilOp::Keep, sq::StencilOp::Invert, sq::StencilOp::Keep);

    // only render where there is geometry
    context.set_stencil_params(sq::CompareFunc::Equal, 0x01, 0x01, 0x02);

    // clear light volume stencil bit
    context.clear_stencil(0x00, 0x02);

    context.bind_program(PROG_Main_Stencil);

    PROG_Main_Stencil.update(0, light.matrix);

    volume.apply_to_context(context);
    volume.draw(context);

    context.bind_program(program);

    // disable depth test for light accumulation
    context.set_state(sq::DepthTest::Disable);

    // only render over the stencil volume bit
    context.set_state(sq::StencilTest::Keep);
    context.set_stencil_params(sq::CompareFunc::Equal, 0x02, 0x02, 0x00);

    context.bind_buffer(light.ubo, sq::BufTarget::Uniform, 1u);
    context.bind_texture(light.tex, 8u);

    // render full screen lighting quad
    context.bind_vertexarray_dummy();
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);
}
