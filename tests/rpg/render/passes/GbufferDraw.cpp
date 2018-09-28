#include <sqee/gl/Context.hpp>

#include <sqee/redist/gl_loader.hpp>

#include "GbufferDraw.hpp"

using Context = sq::Context;
using namespace sqt::render;

//============================================================================//

namespace { // anonymous

void impl_activate_material(Context& context, const sq::Program& program, const sq::Material& material)
{
    const bool hasDiff = material.has_diffuse_texture();
    const bool hasNorm = material.has_normal_texture();
    const bool hasSpec = material.has_specular_texture();
    program.update(2, Vec3I{ hasDiff, hasNorm, hasSpec });

    if (hasDiff == false) program.update(3, material.get_diffuse_colour());
    if (hasSpec == false) program.update(4, material.get_specular_colour());

    if (hasDiff == true) context.bind_Texture(material.get_diffuse_texture(),  0u);
    if (hasNorm == true) context.bind_Texture(material.get_normal_texture(),   1u);
    if (hasSpec == true) context.bind_Texture(material.get_specular_texture(), 2u);
}

} // anonymous namespace

//============================================================================//

GbufferPasses::GbufferPasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    FB_MainGbuffer.draw_buffers
    ({
        gl::COLOR_ATTACHMENT0, gl::COLOR_ATTACHMENT1,
        gl::COLOR_ATTACHMENT2, gl::COLOR_ATTACHMENT3
    });

    processor.load_vertex(PROG_ModelPass_Simple, "gbuffer/ModelPass/Simple_vs");
    processor.load_vertex(PROG_ModelPass_Skelly, "gbuffer/ModelPass/Skelly_vs");
    processor.load_vertex(PROG_DecalPass_Decal, "gbuffer/DecalPass/Decal_vs");

    processor.load_fragment(PROG_ModelPass_Simple, "gbuffer/ModelPass/Model_fs");
    processor.load_fragment(PROG_ModelPass_Skelly, "gbuffer/ModelPass/Model_fs");
    processor.load_fragment(PROG_DecalPass_Decal, "gbuffer/DecalPass/Decal_fs");

    PROG_ModelPass_Simple.link_program_stages();
    PROG_ModelPass_Skelly.link_program_stages();
    PROG_DecalPass_Decal.link_program_stages();
}

//============================================================================//

void GbufferPasses::update_options()
{
    FB_MainGbuffer.attach(gl::COLOR_ATTACHMENT0, textures.Gbuffer_MainDiff);
    FB_MainGbuffer.attach(gl::COLOR_ATTACHMENT1, textures.Gbuffer_MainSurf);
    FB_MainGbuffer.attach(gl::COLOR_ATTACHMENT2, textures.Gbuffer_MainNorm);
    FB_MainGbuffer.attach(gl::COLOR_ATTACHMENT3, textures.Gbuffer_MainSpec);
    FB_MainGbuffer.attach(gl::DEPTH_STENCIL_ATTACHMENT, textures.Depth_FullSize);
}

//============================================================================//

void GbufferPasses::render(const data::GbufferPasses& data)
{
    // bind and clear framebuffer
    context.set_ViewPort(options.Window_Size);
    context.bind_FrameBuffer(FB_MainGbuffer);
    context.clear_Colour(Vec4F(0.f));

    // render sub passes
    impl_render_ModelPasses(data.modelPasses);
    impl_render_DecalPasses(data.decalPasses);
}

//============================================================================//

void GbufferPasses::impl_render_ModelPasses(const data::GbufferModelPasses& data)
{
    // use back face culling for all models
    context.set_state(Context::Cull_Face::Back);

    // test if model is visible in the depth buffer
    context.set_state(Context::Depth_Compare::Equal);
    context.set_state(Context::Depth_Test::Keep);

    // disable stencil test and blending
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Disable);

    // render sub passes
    impl_render_ModelSimplePass(data.simplePass);
    impl_render_ModelSkellyPass(data.skellyPass);

    // revert to standard depth comparison
    context.set_state(Context::Depth_Compare::LessEqual);
}

//============================================================================//

void GbufferPasses::impl_render_ModelSimplePass(const data::GbufferModelSimplePass& data)
{
    /// Renders the gbuffer for all simple models.
    /// Requires the completion of depth passes.

    //--------------------------------------------------------//

    context.bind_Program(PROG_ModelPass_Simple);

    for (const auto& [material, models] : data.partMap)
    {
        impl_activate_material(context, PROG_ModelPass_Simple, *material);

        for (const auto& model : models)
        {
            PROG_ModelPass_Simple.update(0, model->matrix);
            PROG_ModelPass_Simple.update(1, model->normMat);

            if (model->mirror) context.set_state(Context::Cull_Face::Front);
            else context.set_state(Context::Cull_Face::Back);

            context.bind_VertexArray(model->mesh.get_vao());
            model->mesh.draw_partial(model.index);
        }
    }
}

//============================================================================//

void GbufferPasses::impl_render_ModelSkellyPass(const data::GbufferModelSkellyPass& data)
{
    /// Renders the gbuffer for all skelly models.
    /// Requires the completion of depth passes.

    //--------------------------------------------------------//

    context.bind_Program(PROG_ModelPass_Skelly);

    for (const auto& [material, models] : data.partMap)
    {
        impl_activate_material(context, PROG_ModelPass_Skelly, *material);

        for (const auto& model : models)
        {
            PROG_ModelPass_Skelly.update(0, model->matrix);
            PROG_ModelPass_Skelly.update(1, model->normMat);

            if (model->mirror) context.set_state(Context::Cull_Face::Front);
            else context.set_state(Context::Cull_Face::Back);

            context.bind_VertexArray(model->mesh.get_vao());
            context.bind_UniformBuffer(model->ubo, 1u);
            model->mesh.draw_partial(model.index);
        }
    }
}

//============================================================================//

void GbufferPasses::impl_render_DecalPasses(const data::GbufferDecalPasses& data)
{
    // decal boxes require front face culling
    context.set_state(Context::Cull_Face::Front);

    // disable depth, stencil and blending
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Disable);

    // bind gbuffer textures for manual blending
    context.bind_Texture(textures.Gbuffer_MainDiff, 3u);
    context.bind_Texture(textures.Gbuffer_MainSurf, 4u);
    context.bind_Texture(textures.Gbuffer_MainNorm, 5u);
    context.bind_Texture(textures.Gbuffer_MainSpec, 6u);
    context.bind_Texture(textures.Depth_FullSize, 7u);

    // render sub passes
    impl_render_DecalBasicPass(data.basicPass);
}

//============================================================================//

void GbufferPasses::impl_render_DecalBasicPass(const data::GbufferDecalBasicPass& data)
{
    context.bind_Program(PROG_DecalPass_Decal);

    for (const auto& [material, decals] : data.decalMap)
    {
        impl_activate_material(context, PROG_DecalPass_Decal, *material);

        for (const auto& decal : decals)
        {
            PROG_DecalPass_Decal.update(0, decal.matrix);
            PROG_DecalPass_Decal.update(1, decal.invMatrix);
            PROG_DecalPass_Decal.update(3, decal.alpha);

            volumes.Unit_Cube.bind_and_draw(context);
        }
    }
}
