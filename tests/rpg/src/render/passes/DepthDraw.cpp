#include <sqee/gl/Context.hpp>

#include "DepthDraw.hpp"

using namespace sqt::render;

//============================================================================//

DepthPasses::DepthPasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    processor.load_vertex(PROG_DepthPass_SimpleSolid, "shaders/gbuffer/DepthPass/SimpleSolid_vs.glsl", {});
    processor.load_vertex(PROG_DepthPass_SimplePunch, "shaders/gbuffer/DepthPass/SimplePunch_vs.glsl", {});
    processor.load_vertex(PROG_DepthPass_SkellySolid, "shaders/gbuffer/DepthPass/SkellySolid_vs.glsl", {});
    processor.load_vertex(PROG_DepthPass_SkellyPunch, "shaders/gbuffer/DepthPass/SkellyPunch_vs.glsl", {});

    processor.load_fragment(PROG_DepthPass_SimplePunch, "shaders/gbuffer/DepthPass/ModelPunch_fs.glsl", {});
    processor.load_fragment(PROG_DepthPass_SkellyPunch, "shaders/gbuffer/DepthPass/ModelPunch_fs.glsl", {});

    PROG_DepthPass_SimpleSolid.link_program_stages();
    PROG_DepthPass_SimplePunch.link_program_stages();
    PROG_DepthPass_SkellySolid.link_program_stages();
    PROG_DepthPass_SkellyPunch.link_program_stages();
}

//============================================================================//

void DepthPasses::update_options()
{
    INFO_fullSize = options.Window_Size;
    INFO_halfSize = options.Window_Size / 2u;
    INFO_qterSize = options.Window_Size / 4u;

    FB_FullDepth.attach(sq::FboAttach::DepthStencil, textures.Depth_FullSize);
    FB_HalfDepth.attach(sq::FboAttach::DepthStencil, textures.Depth_HalfSize);
    FB_QterDepth.attach(sq::FboAttach::DepthStencil, textures.Depth_QterSize);
}

//============================================================================//

void DepthPasses::render(const data::DepthPasses& data)
{
    // bind and clear framebuffer
    context.set_ViewPort(options.Window_Size);
    context.bind_framebuffer(FB_FullDepth, sq::FboTarget::Both);
    context.clear_depth_stencil(1.0, 0x00, 0xFF);

    // enable depth write and disable blending
    context.set_state(sq::DepthTest::Replace);
    context.set_state(sq::BlendMode::Disable);

    // always write the stencil buffer geometry bit
    context.set_state(sq::StencilTest::Replace);
    context.set_stencil_params(sq::CompareFunc::Always, 0x01, 0x00, 0x01);

    // perform sub passes
    impl_render_ModelSimplePass(data.modelSimplePass);
    impl_render_ModelSkellyPass(data.modelSkellyPass);

    //-- blit depth stencil to half and quarter textures -----//

    FB_FullDepth.blit(FB_HalfDepth, INFO_fullSize, INFO_halfSize, sq::BlitMask::DepthStencil, false);

    FB_FullDepth.blit(FB_QterDepth, INFO_fullSize, INFO_qterSize, sq::BlitMask::DepthStencil, false);
}

//============================================================================//

void DepthPasses::impl_render_ModelSimplePass(const data::DepthModelSimplePass& data)
{
    /// Renders the depth and stencil for all simple models.
    /// Does not require completion of any other passes.

    //--------------------------------------------------------//

    context.bind_program(PROG_DepthPass_SimpleSolid);

    for (const auto& model : data.solidFullVec)
    {
        PROG_DepthPass_SimpleSolid.update(0, model.matrix);

        if (model.mirror) context.set_state(sq::CullFace::Front);
        else context.set_state(sq::CullFace::Back);

        model.mesh.apply_to_context(context);
        model.mesh.draw_complete(context);
    }

    for (const auto& model : data.solidPartVec)
    {
        PROG_DepthPass_SimpleSolid.update(0, model.matrix);

        if (model.mirror) context.set_state(sq::CullFace::Front);
        else context.set_state(sq::CullFace::Back);

        model.mesh.apply_to_context(context);
        model.mesh.draw_submesh(context, model.index);
    }

    //--------------------------------------------------------//

    context.bind_program(PROG_DepthPass_SimplePunch);

    for (const auto& [maskTexture, models] : data.punchPartMap)
    {
        context.bind_texture(*maskTexture, 0u);

        for (const auto& model : models)
        {
            PROG_DepthPass_SimplePunch.update(0, model.matrix);

            if (model.mirror) context.set_state(sq::CullFace::Front);
            else context.set_state(sq::CullFace::Back);

            model.mesh.apply_to_context(context);
            model.mesh.draw_submesh(context, model.index);
        }
    }
}

//============================================================================//

void DepthPasses::impl_render_ModelSkellyPass(const data::DepthModelSkellyPass& data)
{
    /// Renders the depth and stencil for all skelly models.
    /// Does not require completion of any other passes.

    //--------------------------------------------------------//

    context.bind_program(PROG_DepthPass_SkellySolid);

    for (const auto& model : data.solidFullVec)
    {
        PROG_DepthPass_SkellySolid.update(0, model.matrix);

        if (model.mirror) context.set_state(sq::CullFace::Front);
        else context.set_state(sq::CullFace::Back);

        context.bind_buffer(model.ubo, sq::BufTarget::Uniform, 1u);
        model.mesh.apply_to_context(context);
        model.mesh.draw_complete(context);
    }

    for (const auto& model : data.solidPartVec)
    {
        PROG_DepthPass_SkellySolid.update(0, model.matrix);

        if (model.mirror) context.set_state(sq::CullFace::Front);
        else context.set_state(sq::CullFace::Back);

        context.bind_buffer(model.ubo, sq::BufTarget::Uniform, 1u);
        model.mesh.apply_to_context(context);
        model.mesh.draw_submesh(context, model.index);
    }

    //--------------------------------------------------------//

    context.bind_program(PROG_DepthPass_SkellyPunch);

    for (const auto& textureVecPair : data.punchPartMap)
    {
        context.bind_texture(*textureVecPair.first, 0u);

        for (const auto& model : textureVecPair.second)
        {
            PROG_DepthPass_SkellyPunch.update(0, model.matrix);

            if (model.mirror) context.set_state(sq::CullFace::Front);
            else context.set_state(sq::CullFace::Back);

            context.bind_buffer(model.ubo, sq::BufTarget::Uniform, 1u);
            model.mesh.apply_to_context(context);
            model.mesh.draw_submesh(context, model.index);
        }
    }
}
