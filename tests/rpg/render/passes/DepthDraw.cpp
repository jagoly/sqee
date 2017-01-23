#include <sqee/gl/Context.hpp>

#include "../../Options.hpp"
#include "DepthDraw.hpp"

using Context = sq::Context;
using namespace sqt::render;

//============================================================================//

DepthPasses::DepthPasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    VS_DepthPass_SimpleSolid.add_uniform("matrix"); // Mat4F
    VS_DepthPass_SkellySolid.add_uniform("matrix"); // Mat4F
    VS_DepthPass_SimplePunch.add_uniform("matrix"); // Mat4F
    VS_DepthPass_SkellyPunch.add_uniform("matrix"); // Mat4F

    shaders.preprocs(VS_DepthPass_SimpleSolid, "gbuffer/DepthPass/SimpleSolid_vs");
    shaders.preprocs(VS_DepthPass_SkellySolid, "gbuffer/DepthPass/SkellySolid_vs");
    shaders.preprocs(VS_DepthPass_SimplePunch, "gbuffer/DepthPass/SimplePunch_vs");
    shaders.preprocs(VS_DepthPass_SkellyPunch, "gbuffer/DepthPass/SkellyPunch_vs");
    shaders.preprocs(FS_DepthPass_ModelPunch, "gbuffer/DepthPass/ModelPunch_fs");
}

//============================================================================//

void DepthPasses::update_options()
{
    INFO_fullSize = options.Window_Size;
    INFO_halfSize = options.Window_Size / 2u;
    INFO_qterSize = options.Window_Size / 4u;

    FB_FullDepth.attach(gl::DEPTH_STENCIL_ATTACHMENT, textures.Depth_FullSize);
    FB_HalfDepth.attach(gl::DEPTH_STENCIL_ATTACHMENT, textures.Depth_HalfSize);
    FB_QterDepth.attach(gl::DEPTH_STENCIL_ATTACHMENT, textures.Depth_QterSize);
}

//============================================================================//

void DepthPasses::render(const data::DepthPasses& data)
{
    // bind and clear framebuffer
    context.set_ViewPort(options.Window_Size);
    context.bind_FrameBuffer(FB_FullDepth);
    context.clear_Depth_Stencil();

    // use back face culling for all models
    context.set_state(Context::Cull_Face::Back);

    // enable depth write and disable blending
    context.set_state(Context::Depth_Test::Replace);
    context.set_state(Context::Blend_Mode::Disable);

    // always write the stencil buffer geometry bit
    context.set_state(Context::Stencil_Test::Replace);
    context.set_Stencil_Params(gl::ALWAYS, 1, 0, 1);

    ////// sub passes //////
    impl_render_ModelSimplePass(data.modelSimplePass);
    impl_render_ModelSkellyPass(data.modelSkellyPass);

    // blit the main full size depth stencil texture to half and quarter textures /////
    FB_FullDepth.blit(FB_HalfDepth, INFO_fullSize, INFO_halfSize, gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT, gl::NEAREST);
    FB_FullDepth.blit(FB_QterDepth, INFO_fullSize, INFO_qterSize, gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT, gl::NEAREST);
}

//============================================================================//

void DepthPasses::impl_render_ModelSimplePass(const data::DepthModelSimplePass& data)
{
    /// Renders the depth and stencil for all simple models.
    /// Does not require completion of any other passes.
    /// Most state is set by the calling methods.

    //========================================================//

    // Render solid simple models /////

    context.use_Shader_Vert(VS_DepthPass_SimpleSolid);
    context.disable_shader_stage_fragment();

    for (const auto& model : data.solidFullVec)
    {
        VS_DepthPass_SimpleSolid.update<Mat4F>("matrix", model.matrix);

        if (model.mirror) context.set_state(Context::Cull_Face::Front);
        else context.set_state(Context::Cull_Face::Back);

        context.bind_VertexArray(model.mesh.get_vao());
        model.mesh.draw_complete();
    }

    for (const auto& model : data.solidPartVec)
    {
        VS_DepthPass_SimpleSolid.update<Mat4F>("matrix", model.matrix);

        if (model.mirror) context.set_state(Context::Cull_Face::Front);
        else context.set_state(Context::Cull_Face::Back);

        context.bind_VertexArray(model.mesh.get_vao());
        model.mesh.draw_partial(model.index);
    }

    //========================================================//

    // Render punch simple models /////

    context.use_Shader_Vert(VS_DepthPass_SimplePunch);
    context.use_Shader_Frag(FS_DepthPass_ModelPunch);

    for (const auto& textureVecPair : data.punchPartMap)
    {
        context.bind_Texture(*textureVecPair.first, 0u);

        for (const auto& model : textureVecPair.second)
        {
            VS_DepthPass_SimplePunch.update<Mat4F>("matrix", model.matrix);

            if (model.mirror) context.set_state(Context::Cull_Face::Front);
            else context.set_state(Context::Cull_Face::Back);

            context.bind_VertexArray(model.mesh.get_vao());
            model.mesh.draw_partial(model.index);
        }
    }
}

//============================================================================//

void DepthPasses::impl_render_ModelSkellyPass(const data::DepthModelSkellyPass& data)
{
    /// Renders the depth and stencil for all skelly models.
    /// Does not require completion of any other passes.
    /// Most state is set by the calling methods.

    //========================================================//

    // render solid skelly models /////

    context.use_Shader_Vert(VS_DepthPass_SkellySolid);
    context.disable_shader_stage_fragment();

    for (const auto& model : data.solidFullVec)
    {
        VS_DepthPass_SkellySolid.update<Mat4F>("matrix", model.matrix);

        if (model.mirror) context.set_state(Context::Cull_Face::Front);
        else context.set_state(Context::Cull_Face::Back);

        context.bind_VertexArray(model.mesh.get_vao());
        context.bind_UniformBuffer(model.ubo, 1u);
        model.mesh.draw_complete();
    }

    for (const auto& model : data.solidPartVec)
    {
        VS_DepthPass_SkellySolid.update<Mat4F>("matrix", model.matrix);

        if (model.mirror) context.set_state(Context::Cull_Face::Front);
        else context.set_state(Context::Cull_Face::Back);

        context.bind_VertexArray(model.mesh.get_vao());
        context.bind_UniformBuffer(model.ubo, 1u);
        model.mesh.draw_partial(model.index);
    }

    //========================================================//

    // render punch skelly models /////

    context.use_Shader_Vert(VS_DepthPass_SkellyPunch);
    context.use_Shader_Frag(FS_DepthPass_ModelPunch);

    for (const auto& textureVecPair : data.punchPartMap)
    {
        context.bind_Texture(*textureVecPair.first, 0u);

        for (const auto& model : textureVecPair.second)
        {
            VS_DepthPass_SkellyPunch.update<Mat4F>("matrix", model.matrix);

            if (model.mirror) context.set_state(Context::Cull_Face::Front);
            else context.set_state(Context::Cull_Face::Back);

            context.bind_VertexArray(model.mesh.get_vao());
            context.bind_UniformBuffer(model.ubo, 1u);
            model.mesh.draw_partial(model.index);
        }
    }
}

//============================================================================//
