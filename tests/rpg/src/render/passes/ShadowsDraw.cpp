#include <sqee/gl/Context.hpp>

#include "ShadowsDraw.hpp"

using namespace sqt::render;

//============================================================================//

ShadowsPasses::ShadowsPasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    processor.load_vertex(PROG_SimpleSolid, "shaders/shadows/SimpleSolid_vs.glsl", {});
    processor.load_vertex(PROG_SimplePunch, "shaders/shadows/SimplePunch_vs.glsl", {});
    processor.load_vertex(PROG_SkellySolid, "shaders/shadows/SkellySolid_vs.glsl", {});
    processor.load_vertex(PROG_SkellyPunch, "shaders/shadows/SkellyPunch_vs.glsl", {});

    processor.load_fragment(PROG_SimplePunch, "shaders/shadows/ModelPunch_fs.glsl", {});
    processor.load_fragment(PROG_SkellyPunch, "shaders/shadows/ModelPunch_fs.glsl", {});

    PROG_SimpleSolid.link_program_stages();
    PROG_SimplePunch.link_program_stages();
    PROG_SkellySolid.link_program_stages();
    PROG_SkellyPunch.link_program_stages();
}

//============================================================================//

void ShadowsPasses::update_options()
{

}

//============================================================================//

void ShadowsPasses::render(const data::ShadowsPasses& data)
{
    context.set_state(sq::CullFace::Back);
    context.set_state(sq::DepthTest::Replace);
    context.set_state(sq::StencilTest::Disable);
    context.set_state(sq::BlendMode::Disable);

    //-- depth clamp orthographic projections ----------------//

    context.set_state(sq::DepthClamp::Enable);

    for (const auto& pass : data.cascPassVec) impl_render_ShadowsPass(pass);
    for (const auto& pass : data.orthoPassVec) impl_render_ShadowsPass(pass);

    //-- don't depth clamp perspective projections -----------//

    context.set_state(sq::DepthClamp::Disable);

    for (const auto& pass : data.pointPassVec) impl_render_ShadowsPass(pass);
    for (const auto& pass : data.spotPassVec) impl_render_ShadowsPass(pass);
}

//============================================================================//

void ShadowsPasses::impl_render_ShadowsPass(const data::ShadowsPass& pass)
{
    context.set_ViewPort(pass.viewPort);
    context.bind_framebuffer(pass.fbo);
    context.clear_depth(1.0);

    const auto& dp = pass.depthPasses;

    //-- render solid simple models --------------------------//

    context.bind_program(PROG_SimpleSolid);

    for (const auto& model : dp.modelSimplePass.solidFullVec)
    {
        PROG_SimpleSolid.update(0, model.matrix);

        if (model.mirror) context.set_state(sq::CullFace::Front);
        else context.set_state(sq::CullFace::Back);

        model.mesh.apply_to_context(context);
        model.mesh.draw_complete(context);
    }

    for (const auto& model : dp.modelSimplePass.solidPartVec)
    {
        PROG_SimpleSolid.update(0, model.matrix);

        if (model.mirror) context.set_state(sq::CullFace::Front);
        else context.set_state(sq::CullFace::Back);

        model.mesh.apply_to_context(context);
        model.mesh.draw_submesh(context, model.index);
    }

    //-- render solid skelly models --------------------------//

    context.bind_program(PROG_SkellySolid);

    for (const auto& model : dp.modelSkellyPass.solidFullVec)
    {
        PROG_SkellySolid.update(0, model.matrix);

        if (model.mirror) context.set_state(sq::CullFace::Front);
        else context.set_state(sq::CullFace::Back);

        context.bind_buffer(model.ubo, sq::BufTarget::Uniform, 1u);
        model.mesh.apply_to_context(context);
        model.mesh.draw_complete(context);
    }

    for (const auto& model : dp.modelSkellyPass.solidPartVec)
    {
        PROG_SkellySolid.update(0, model.matrix);

        if (model.mirror) context.set_state(sq::CullFace::Front);
        else context.set_state(sq::CullFace::Back);

        context.bind_buffer(model.ubo, sq::BufTarget::Uniform, 1u);
        model.mesh.apply_to_context(context);
        model.mesh.draw_submesh(context, model.index);
    }

    //-- render punch simple models --------------------------//

    context.bind_program(PROG_SimplePunch);

    for (const auto& textureVecPair : dp.modelSimplePass.punchPartMap)
    {
        context.bind_texture(*textureVecPair.first, 0u);

        for (const auto& model : textureVecPair.second)
        {
            PROG_SimplePunch.update(0, model.matrix);

            if (model.mirror) context.set_state(sq::CullFace::Front);
            else context.set_state(sq::CullFace::Back);

            model.mesh.apply_to_context(context);
            model.mesh.draw_submesh(context, model.index);
        }
    }

    //-- render punch skelly models --------------------------//

    context.bind_program(PROG_SkellyPunch);

    for (const auto& textureVecPair : dp.modelSkellyPass.punchPartMap)
    {
        context.bind_texture(*textureVecPair.first, 0u);

        for (const auto& model : textureVecPair.second)
        {
            PROG_SkellyPunch.update(0, model.matrix);

            if (model.mirror) context.set_state(sq::CullFace::Front);
            else context.set_state(sq::CullFace::Back);

            context.bind_buffer(model.ubo, sq::BufTarget::Uniform, 1u);
            model.mesh.apply_to_context(context);
            model.mesh.draw_submesh(context, model.index);
        }
    }
}
