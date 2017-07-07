#include <sqee/gl/Context.hpp>

#include "ShadowsDraw.hpp"

using Context = sq::Context;
using namespace sqt::render;

//============================================================================//

ShadowsPasses::ShadowsPasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    processor.load_vertex(PROG_SimpleSolid, "shadows/SimpleSolid_vs");
    processor.load_vertex(PROG_SimplePunch, "shadows/SimplePunch_vs");
    processor.load_vertex(PROG_SkellySolid, "shadows/SkellySolid_vs");
    processor.load_vertex(PROG_SkellyPunch, "shadows/SkellyPunch_vs");

    processor.load_fragment(PROG_SimplePunch, "shadows/ModelPunch_fs");
    processor.load_fragment(PROG_SkellyPunch, "shadows/ModelPunch_fs");

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
    context.set_state(Context::Cull_Face::Back);
    context.set_state(Context::Depth_Test::Replace);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Disable);

    // depth clamp orthographic projections
    context.set_state(Context::Depth_Clamp::Enable);
    for (const auto& pass : data.cascPassVec) impl_render_ShadowsPass(pass);
    for (const auto& pass : data.orthoPassVec) impl_render_ShadowsPass(pass);

    // don't depth clamp perspective projections
    context.set_state(Context::Depth_Clamp::Disable);
    for (const auto& pass : data.pointPassVec) impl_render_ShadowsPass(pass);
    for (const auto& pass : data.spotPassVec) impl_render_ShadowsPass(pass);

    context.bind_Program_default();
}

//============================================================================//

void ShadowsPasses::impl_render_ShadowsPass(const data::ShadowsPass& pass)
{
    context.set_ViewPort(pass.viewPort);
    context.bind_FrameBuffer(pass.fbo);
    context.clear_Depth(1.0);

    const auto& dp = pass.depthPasses;

    //========================================================//

    // render solid simple models /////

    context.bind_Program(PROG_SimpleSolid);

    for (const auto& model : dp.modelSimplePass.solidFullVec)
    {
        PROG_SimpleSolid.update(0, model.matrix);

        if (model.mirror) context.set_state(Context::Cull_Face::Front);
        else context.set_state(Context::Cull_Face::Back);

        context.bind_VertexArray(model.mesh.get_vao());
        model.mesh.draw_complete();
    }

    for (const auto& model : dp.modelSimplePass.solidPartVec)
    {
        PROG_SimpleSolid.update(0, model.matrix);

        if (model.mirror) context.set_state(Context::Cull_Face::Front);
        else context.set_state(Context::Cull_Face::Back);

        context.bind_VertexArray(model.mesh.get_vao());
        model.mesh.draw_partial(model.index);
    }

    //========================================================//

    // render solid skelly models /////

    context.bind_Program(PROG_SkellySolid);

    for (const auto& model : dp.modelSkellyPass.solidFullVec)
    {
        PROG_SkellySolid.update(0, model.matrix);

        if (model.mirror) context.set_state(Context::Cull_Face::Front);
        else context.set_state(Context::Cull_Face::Back);

        context.bind_VertexArray(model.mesh.get_vao());
        context.bind_UniformBuffer(model.ubo, 1u);
        model.mesh.draw_complete();
    }

    for (const auto& model : dp.modelSkellyPass.solidPartVec)
    {
        PROG_SkellySolid.update(0, model.matrix);

        if (model.mirror) context.set_state(Context::Cull_Face::Front);
        else context.set_state(Context::Cull_Face::Back);

        context.bind_VertexArray(model.mesh.get_vao());
        context.bind_UniformBuffer(model.ubo, 1u);
        model.mesh.draw_partial(model.index);
    }

    //========================================================//

    // render punch simple models /////

    context.bind_Program(PROG_SimplePunch);

    for (const auto& textureVecPair : dp.modelSimplePass.punchPartMap)
    {
        context.bind_Texture(*textureVecPair.first, 0u);

        for (const auto& model : textureVecPair.second)
        {
            PROG_SimplePunch.update(0, model.matrix);

            if (model.mirror) context.set_state(Context::Cull_Face::Front);
            else context.set_state(Context::Cull_Face::Back);

            context.bind_VertexArray(model.mesh.get_vao());
            model.mesh.draw_partial(model.index);
        }
    }

    //========================================================//

    // render punch skelly models /////

    context.bind_Program(PROG_SkellyPunch);

    for (const auto& textureVecPair : dp.modelSkellyPass.punchPartMap)
    {
        context.bind_Texture(*textureVecPair.first, 0u);

        for (const auto& model : textureVecPair.second)
        {
            PROG_SkellyPunch.update(0, model.matrix);

            if (model.mirror) context.set_state(Context::Cull_Face::Front);
            else context.set_state(Context::Cull_Face::Back);

            context.bind_VertexArray(model.mesh.get_vao());
            context.bind_UniformBuffer(model.ubo, 1u);
            model.mesh.draw_partial(model.index);
        }
    }
}

//============================================================================//
