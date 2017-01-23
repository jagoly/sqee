#include <sqee/gl/Context.hpp>

#include "ShadowsDraw.hpp"

using Context = sq::Context;
using namespace sqt::render;

//============================================================================//

ShadowsPasses::ShadowsPasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    VS_SimpleSolid.add_uniform("matrix"); // Mat4F
    VS_SkellySolid.add_uniform("matrix"); // Mat4F
    VS_SimplePunch.add_uniform("matrix"); // Mat4F
    VS_SkellyPunch.add_uniform("matrix"); // Mat4F

    shaders.preprocs(VS_SimpleSolid, "shadows/SimpleSolid_vs");
    shaders.preprocs(VS_SkellySolid, "shadows/SkellySolid_vs");
    shaders.preprocs(VS_SimplePunch, "shadows/SimplePunch_vs");
    shaders.preprocs(VS_SkellyPunch, "shadows/SkellyPunch_vs");
    shaders.preprocs(FS_ModelPunch, "shadows/ModelPunch_fs");
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

    context.use_Shader_Vert(VS_SimpleSolid);
    context.disable_shader_stage_fragment();

    for (const auto& model : dp.modelSimplePass.solidFullVec)
    {
        VS_SimpleSolid.update<Mat4F>("matrix", model.matrix);

        if (model.mirror) context.set_state(Context::Cull_Face::Front);
        else context.set_state(Context::Cull_Face::Back);

        context.bind_VertexArray(model.mesh.get_vao());
        model.mesh.draw_complete();
    }

    for (const auto& model : dp.modelSimplePass.solidPartVec)
    {
        VS_SimpleSolid.update<Mat4F>("matrix", model.matrix);

        if (model.mirror) context.set_state(Context::Cull_Face::Front);
        else context.set_state(Context::Cull_Face::Back);

        context.bind_VertexArray(model.mesh.get_vao());
        model.mesh.draw_partial(model.index);
    }

    //========================================================//

    // render solid skelly models /////

    context.use_Shader_Vert(VS_SkellySolid);
    context.disable_shader_stage_fragment();

    for (const auto& model : dp.modelSkellyPass.solidFullVec)
    {
        VS_SkellySolid.update<Mat4F>("matrix", model.matrix);

        if (model.mirror) context.set_state(Context::Cull_Face::Front);
        else context.set_state(Context::Cull_Face::Back);

        context.bind_VertexArray(model.mesh.get_vao());
        context.bind_UniformBuffer(model.ubo, 1u);
        model.mesh.draw_complete();
    }

    for (const auto& model : dp.modelSkellyPass.solidPartVec)
    {
        VS_SkellySolid.update<Mat4F>("matrix", model.matrix);

        if (model.mirror) context.set_state(Context::Cull_Face::Front);
        else context.set_state(Context::Cull_Face::Back);

        context.bind_VertexArray(model.mesh.get_vao());
        context.bind_UniformBuffer(model.ubo, 1u);
        model.mesh.draw_partial(model.index);
    }

    //========================================================//

    // render punch simple models /////

    context.use_Shader_Vert(VS_SimplePunch);
    context.use_Shader_Frag(FS_ModelPunch);

    for (const auto& textureVecPair : dp.modelSimplePass.punchPartMap)
    {
        context.bind_Texture(*textureVecPair.first, 0u);

        for (const auto& model : textureVecPair.second)
        {
            VS_SimplePunch.update<Mat4F>("matrix", model.matrix);

            if (model.mirror) context.set_state(Context::Cull_Face::Front);
            else context.set_state(Context::Cull_Face::Back);

            context.bind_VertexArray(model.mesh.get_vao());
            model.mesh.draw_partial(model.index);
        }
    }

    //========================================================//

    // render punch skelly models /////

    context.use_Shader_Vert(VS_SkellyPunch);
    context.use_Shader_Frag(FS_ModelPunch);

    for (const auto& textureVecPair : dp.modelSkellyPass.punchPartMap)
    {
        context.bind_Texture(*textureVecPair.first, 0u);

        for (const auto& model : textureVecPair.second)
        {
            VS_SkellyPunch.update<Mat4F>("matrix", model.matrix);

            if (model.mirror) context.set_state(Context::Cull_Face::Front);
            else context.set_state(Context::Cull_Face::Back);

            context.bind_VertexArray(model.mesh.get_vao());
            context.bind_UniformBuffer(model.ubo, 1u);
            model.mesh.draw_partial(model.index);
        }
    }
}

//============================================================================//
