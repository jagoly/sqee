#include <sqee/gl/Context.hpp>

#include "GbufferDraw.hpp"

using namespace sqt::render;

//============================================================================//

namespace { // anonymous

void impl_activate_material(sq::Context& context, const sq::Program& program, const sq::Material& material)
{
//    const bool hasDiff = material.has_diffuse_texture();
//    const bool hasNorm = material.has_normal_texture();
//    const bool hasSpec = material.has_specular_texture();
//    program.update(2, Vec3I{ hasDiff, hasNorm, hasSpec });

//    if (hasDiff == false) program.update(3, material.get_diffuse_colour());
//    if (hasSpec == false) program.update(4, material.get_specular_colour());

//    if (hasDiff == true) context.bind_Texture(material.get_diffuse_texture(),  0u);
//    if (hasNorm == true) context.bind_Texture(material.get_normal_texture(),   1u);
//    if (hasSpec == true) context.bind_Texture(material.get_specular_texture(), 2u);
}

} // anonymous namespace

//============================================================================//

GbufferPasses::GbufferPasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    FB_MainGbuffer.draw_buffers(
    {
        sq::FboAttach::Colour0, sq::FboAttach::Colour1,
        sq::FboAttach::Colour2, sq::FboAttach::Colour3
    });

    processor.load_vertex(PROG_ModelPass_Simple, "shaders/gbuffer/ModelPass/Simple_vs.glsl", {});
    processor.load_vertex(PROG_ModelPass_Skelly, "shaders/gbuffer/ModelPass/Skelly_vs.glsl", {});
    processor.load_vertex(PROG_DecalPass_Decal, "shaders/gbuffer/DecalPass/Decal_vs.glsl", {});

    processor.load_fragment(PROG_ModelPass_Simple, "shaders/gbuffer/ModelPass/Model_fs.glsl", {});
    processor.load_fragment(PROG_ModelPass_Skelly, "shaders/gbuffer/ModelPass/Model_fs.glsl", {});
    processor.load_fragment(PROG_DecalPass_Decal, "shaders/gbuffer/DecalPass/Decal_fs.glsl", {});

    PROG_ModelPass_Simple.link_program_stages();
    PROG_ModelPass_Skelly.link_program_stages();
    PROG_DecalPass_Decal.link_program_stages();
}

//============================================================================//

void GbufferPasses::update_options()
{
    FB_MainGbuffer.attach(sq::FboAttach::Colour0, textures.Gbuffer_MainDiff);
    FB_MainGbuffer.attach(sq::FboAttach::Colour1, textures.Gbuffer_MainSurf);
    FB_MainGbuffer.attach(sq::FboAttach::Colour2, textures.Gbuffer_MainNorm);
    FB_MainGbuffer.attach(sq::FboAttach::Colour3, textures.Gbuffer_MainSpec);
    FB_MainGbuffer.attach(sq::FboAttach::DepthStencil, textures.Depth_FullSize);
}

//============================================================================//

void GbufferPasses::render(const data::GbufferPasses& data)
{
    // bind and clear framebuffer
    context.set_ViewPort(options.Window_Size);
    context.bind_framebuffer(FB_MainGbuffer, sq::FboTarget::Both);
    context.clear_colour(Vec4F(0.f));

    // render sub passes
    impl_render_ModelPasses(data.modelPasses);
    impl_render_DecalPasses(data.decalPasses);
}

//============================================================================//

void GbufferPasses::impl_render_ModelPasses(const data::GbufferModelPasses& data)
{
    // use back face culling for all models
    context.set_state(sq::CullFace::Back);

    // test if model is visible in the depth buffer
    context.set_state(sq::DepthTest::Keep);
    context.set_depth_compare(sq::CompareFunc::Equal);

    // disable stencil test and blending
    context.set_state(sq::StencilTest::Disable);
    context.set_state(sq::BlendMode::Disable);

    // render sub passes
    impl_render_ModelSimplePass(data.simplePass);
    impl_render_ModelSkellyPass(data.skellyPass);

    // revert to standard depth comparison
    context.set_depth_compare(sq::CompareFunc::LessEqual);
}

//============================================================================//

void GbufferPasses::impl_render_ModelSimplePass(const data::GbufferModelSimplePass& data)
{
    /// Renders the gbuffer for all simple models.
    /// Requires the completion of depth passes.

    //--------------------------------------------------------//

    context.bind_program(PROG_ModelPass_Simple);

    for (const auto& [material, models] : data.partMap)
    {
        impl_activate_material(context, PROG_ModelPass_Simple, *material);

        for (const auto& model : models)
        {
            PROG_ModelPass_Simple.update(0, model->matrix);
            PROG_ModelPass_Simple.update(1, model->normMat);

            if (model->mirror) context.set_state(sq::CullFace::Front);
            else context.set_state(sq::CullFace::Back);

            model->mesh.apply_to_context(context);
            model->mesh.draw_submesh(context, model.index);
        }
    }
}

//============================================================================//

void GbufferPasses::impl_render_ModelSkellyPass(const data::GbufferModelSkellyPass& data)
{
    /// Renders the gbuffer for all skelly models.
    /// Requires the completion of depth passes.

    //--------------------------------------------------------//

    context.bind_program(PROG_ModelPass_Skelly);

    for (const auto& [material, models] : data.partMap)
    {
        impl_activate_material(context, PROG_ModelPass_Skelly, *material);

        for (const auto& model : models)
        {
            PROG_ModelPass_Skelly.update(0, model->matrix);
            PROG_ModelPass_Skelly.update(1, model->normMat);

            if (model->mirror) context.set_state(sq::CullFace::Front);
            else context.set_state(sq::CullFace::Back);

            context.bind_buffer(model->ubo, sq::BufTarget::Uniform, 1u);
            model->mesh.apply_to_context(context);
            model->mesh.draw_submesh(context, model.index);
        }
    }
}

//============================================================================//

void GbufferPasses::impl_render_DecalPasses(const data::GbufferDecalPasses& data)
{
    // decal boxes require front face culling
    context.set_state(sq::CullFace::Front);

    // disable depth, stencil and blending
    context.set_state(sq::DepthTest::Disable);
    context.set_state(sq::StencilTest::Disable);
    context.set_state(sq::BlendMode::Disable);

    // bind gbuffer textures for manual blending
    context.bind_texture(textures.Gbuffer_MainDiff, 3u);
    context.bind_texture(textures.Gbuffer_MainSurf, 4u);
    context.bind_texture(textures.Gbuffer_MainNorm, 5u);
    context.bind_texture(textures.Gbuffer_MainSpec, 6u);
    context.bind_texture(textures.Depth_FullSize, 7u);

    // render sub passes
    impl_render_DecalBasicPass(data.basicPass);
}

//============================================================================//

void GbufferPasses::impl_render_DecalBasicPass(const data::GbufferDecalBasicPass& data)
{
    context.bind_program(PROG_DecalPass_Decal);

    for (const auto& [material, decals] : data.decalMap)
    {
        impl_activate_material(context, PROG_DecalPass_Decal, *material);

        for (const auto& decal : decals)
        {
            PROG_DecalPass_Decal.update(0, decal.matrix);
            PROG_DecalPass_Decal.update(1, decal.invMatrix);
            PROG_DecalPass_Decal.update(3, decal.alpha);

            volumes.Unit_Cube.apply_to_context(context);
            volumes.Unit_Cube.draw(context);
        }
    }
}
