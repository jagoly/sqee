#include <sqee/gl/Context.hpp>

#include "../../Options.hpp"
#include "DepthDraw.hpp"

using Context = sq::Context;
using namespace sqt::render;

//============================================================================//

DepthPasses::DepthPasses(const SharedStuff& stuff) : SharedStuff(stuff)
{
    processor.load_vertex(PROG_DepthPass_SimpleSolid, "gbuffer/DepthPass/SimpleSolid_vs");
    processor.load_vertex(PROG_DepthPass_SimplePunch, "gbuffer/DepthPass/SimplePunch_vs");
    processor.load_vertex(PROG_DepthPass_SkellySolid, "gbuffer/DepthPass/SkellySolid_vs");
    processor.load_vertex(PROG_DepthPass_SkellyPunch, "gbuffer/DepthPass/SkellyPunch_vs");

    processor.load_fragment(PROG_DepthPass_SimplePunch, "gbuffer/DepthPass/ModelPunch_fs");
    processor.load_fragment(PROG_DepthPass_SkellyPunch, "gbuffer/DepthPass/ModelPunch_fs");

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

    // enable depth write and disable blending
    context.set_state(Context::Depth_Test::Replace);
    context.set_state(Context::Blend_Mode::Disable);

    // always write the stencil buffer geometry bit
    context.set_state(Context::Stencil_Test::Replace);
    context.set_Stencil_Params(gl::ALWAYS, 1, 0, 1);

    // perform sub passes
    impl_render_ModelSimplePass(data.modelSimplePass);
    impl_render_ModelSkellyPass(data.modelSkellyPass);

    // blit the main full size depth stencil texture to half and quarter textures
    FB_FullDepth.blit(FB_HalfDepth, INFO_fullSize, INFO_halfSize, gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT, gl::NEAREST);
    FB_FullDepth.blit(FB_QterDepth, INFO_fullSize, INFO_qterSize, gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT, gl::NEAREST);
}

//============================================================================//

void DepthPasses::impl_render_ModelSimplePass(const data::DepthModelSimplePass& data)
{
    /// Renders the depth and stencil for all simple models.
    /// Does not require completion of any other passes.

    //--------------------------------------------------------//

    context.bind_Program(PROG_DepthPass_SimpleSolid);

    for (const auto& model : data.solidFullVec)
    {
        PROG_DepthPass_SimpleSolid.update(0, model.matrix);

        if (model.mirror) context.set_state(Context::Cull_Face::Front);
        else context.set_state(Context::Cull_Face::Back);

        context.bind_VertexArray(model.mesh.get_vao());
        model.mesh.draw_complete();
    }

    for (const auto& model : data.solidPartVec)
    {
        PROG_DepthPass_SimpleSolid.update(0, model.matrix);

        if (model.mirror) context.set_state(Context::Cull_Face::Front);
        else context.set_state(Context::Cull_Face::Back);

        context.bind_VertexArray(model.mesh.get_vao());
        model.mesh.draw_partial(model.index);
    }

    //--------------------------------------------------------//

    context.bind_Program(PROG_DepthPass_SimplePunch);

    for (const auto& [maskTexture, models] : data.punchPartMap)
    {
        context.bind_Texture(*maskTexture, 0u);

        for (const auto& model : models)
        {
            PROG_DepthPass_SimplePunch.update(0, model.matrix);

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

    //--------------------------------------------------------//

    context.bind_Program(PROG_DepthPass_SkellySolid);

    for (const auto& model : data.solidFullVec)
    {
        PROG_DepthPass_SkellySolid.update(0, model.matrix);

        if (model.mirror) context.set_state(Context::Cull_Face::Front);
        else context.set_state(Context::Cull_Face::Back);

        context.bind_VertexArray(model.mesh.get_vao());
        context.bind_UniformBuffer(model.ubo, 1u);
        model.mesh.draw_complete();
    }

    for (const auto& model : data.solidPartVec)
    {
        PROG_DepthPass_SkellySolid.update(0, model.matrix);

        if (model.mirror) context.set_state(Context::Cull_Face::Front);
        else context.set_state(Context::Cull_Face::Back);

        context.bind_VertexArray(model.mesh.get_vao());
        context.bind_UniformBuffer(model.ubo, 1u);
        model.mesh.draw_partial(model.index);
    }

    //--------------------------------------------------------//

    context.bind_Program(PROG_DepthPass_SkellyPunch);

    for (const auto& textureVecPair : data.punchPartMap)
    {
        context.bind_Texture(*textureVecPair.first, 0u);

        for (const auto& model : textureVecPair.second)
        {
            PROG_DepthPass_SkellyPunch.update(0, model.matrix);

            if (model.mirror) context.set_state(Context::Cull_Face::Front);
            else context.set_state(Context::Cull_Face::Back);

            context.bind_VertexArray(model.mesh.get_vao());
            context.bind_UniformBuffer(model.ubo, 1u);
            model.mesh.draw_partial(model.index);
        }
    }
}

//============================================================================//
