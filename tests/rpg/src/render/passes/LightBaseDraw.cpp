#include <sqee/gl/Context.hpp>

#include "LightBaseDraw.hpp"

using namespace sqt::render;

//============================================================================//

LightBasePasses::LightBasePasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    FB_Lighting.draw_buffers({sq::FboAttach::Colour0});

    processor.load_vertex(PROG_Main_Skybox, "shaders/lighting/Main/Skybox_vs.glsl", {});
    processor.load_vertex(PROG_Main_Ambient, "shaders/generic/FullScreen_vs.glsl", {});

    processor.load_fragment(PROG_Main_Skybox, "shaders/lighting/Skybox_fs.glsl", {});

    PROG_Main_Skybox.link_program_stages();
}

//============================================================================//

void LightBasePasses::update_options()
{
    processor.load_fragment(PROG_Main_Ambient, "shaders/lighting/Main/Ambient_fs.glsl", {});

    PROG_Main_Ambient.link_program_stages();

    // re-attach textures to lighting framebuffer
    FB_Lighting.attach(sq::FboAttach::Colour0, textures.Lighting_Main);
    FB_Lighting.attach(sq::FboAttach::DepthStencil, textures.Depth_FullSize);
}

//============================================================================//

void LightBasePasses::render(const data::LightBasePasses& data)
{
    // bind and clear framebuffer
    context.set_ViewPort(options.Window_Size);
    context.bind_framebuffer(FB_Lighting);
    context.clear_colour(Vec4F(0.f));

    // stencil testing for geometry is used
    context.set_state(sq::StencilTest::Keep);

    // disable cull face, depth test and blend
    context.set_state(sq::CullFace::Disable);
    context.set_state(sq::DepthTest::Disable);
    context.set_state(sq::BlendMode::Disable);

    // bind all gbuffer textures to their slots
    context.bind_texture(textures.Gbuffer_MainDiff, 3u);
    context.bind_texture(textures.Gbuffer_MainSurf, 4u);
    context.bind_texture(textures.Gbuffer_MainNorm, 5u);
    context.bind_texture(textures.Gbuffer_MainSpec, 6u);
    context.bind_texture(textures.Depth_FullSize, 7u);

    //-- render sub passes -----------------------------------//

    if (data.skyboxPass != nullptr) impl_render_SkyBoxPass(*data.skyboxPass);
    if (data.ambientPass != nullptr) impl_render_AmbientPass(*data.ambientPass);
}

//============================================================================//

void LightBasePasses::impl_render_SkyBoxPass(const data::LightBaseSkyBoxPass& data)
{
    // only render where there is no geometry
    context.set_stencil_params(sq::CompareFunc::Equal, 0, 1, 0);

    context.bind_program(PROG_Main_Skybox);

    // enable alpha blending
    //context.set_state(Context::Blend_Mode::Alpha);

    context.bind_texture(data.tex, 0u);

    PROG_Main_Skybox.update(0, data.params);

    context.bind_vertexarray_dummy();
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);
}

//============================================================================//

void LightBasePasses::impl_render_AmbientPass(const data::LightBaseAmbientPass& data)
{
    // only render where there is geometry
    context.set_stencil_params(sq::CompareFunc::Equal, 1, 1, 0);

    context.bind_program(PROG_Main_Ambient);

    if (options.SSAO_Quality != 0u)
    {
        // bind textures required for SSAO
        context.bind_texture(textures.Effects_SSAO, 0u);
        context.bind_texture(textures.Depth_HalfSize, 1u);
    }

    PROG_Main_Ambient.update(0, data.colour);

    context.bind_vertexarray_dummy();
    context.draw_arrays(sq::DrawPrimitive::TriangleStrip, 0u, 4u);
}
