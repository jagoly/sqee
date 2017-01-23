#include <sqee/gl/Context.hpp>

#include "../../Options.hpp"
#include "GbufferDraw.hpp"

using Context = sq::Context;
using namespace sqt::render;

//============================================================================//

namespace { // anonymous

void impl_activate_material(Context& context, const sq::Shader& shader, const sq::Material& material)
{
    const bool hasDiff = material.has_diffuse_texture();
    const bool hasNorm = material.has_normal_texture();
    const bool hasSpec = material.has_specular_texture();

    const Vec3I mtrlMode { hasDiff, hasNorm, hasSpec };
    shader.update<Vec3I>("mtrlMode", mtrlMode);

    if (hasDiff == false) shader.update<Vec3F>("colourDiff", material.get_diffuse_colour());
    if (hasSpec == false) shader.update<Vec3F>("colourSpec", material.get_specular_colour());

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

    VS_ModelPass_Simple.add_uniform("matrix"); // Mat4F
    VS_ModelPass_Simple.add_uniform("normMat"); // Mat3F
    VS_ModelPass_Skelly.add_uniform("matrix"); // Mat4F
    VS_ModelPass_Skelly.add_uniform("normMat"); // Mat3F

    FS_ModelPass_Model.add_uniform("mtrlMode"); // Vec3I
    FS_ModelPass_Model.add_uniform("colourDiff"); // Vec3F
    FS_ModelPass_Model.add_uniform("colourSpec"); // Vec3F

    shaders.preprocs(VS_ModelPass_Simple, "gbuffer/ModelPass/Simple_vs");
    shaders.preprocs(VS_ModelPass_Skelly, "gbuffer/ModelPass/Skelly_vs");
    shaders.preprocs(FS_ModelPass_Model, "gbuffer/ModelPass/Model_fs");

    VS_DecalPass_Decal.add_uniform("modelView"); // Mat4F
    FS_DecalPass_Decal.add_uniform("invModelView"); // Mat4F
    FS_DecalPass_Decal.add_uniform("d_n_s"); // Vec3I
    FS_DecalPass_Decal.add_uniform("alpha"); // float

    shaders.preprocs(VS_DecalPass_Decal, "gbuffer/DecalPass/Decal_vs");
    shaders.preprocs(FS_DecalPass_Decal, "gbuffer/DecalPass/Decal_fs");
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

    ////// sub passes //////
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

    ////// sub passes //////
    impl_render_ModelSimplePass(data.simplePass);
    impl_render_ModelSkellyPass(data.skellyPass);

    // revert to standard depth comparison
    context.set_state(Context::Depth_Compare::LessEqual);
}

//============================================================================//

void GbufferPasses::impl_render_ModelSimplePass(const data::GbufferModelSimplePass& data)
{
    context.use_Shader_Vert(VS_ModelPass_Simple);
    context.use_Shader_Frag(FS_ModelPass_Model);

    for (const auto& materialVecPair : data.partMap)
    {
        const auto& mtrl = materialVecPair.first;
        const auto& models = materialVecPair.second;

        impl_activate_material(context, FS_ModelPass_Model, *mtrl);

        for (const auto& model : models)
        {
            VS_ModelPass_Simple.update<Mat4F>("matrix", model->matrix);
            VS_ModelPass_Simple.update<Mat3F>("normMat", model->normMat);

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
    context.use_Shader_Vert(VS_ModelPass_Skelly);
    context.use_Shader_Frag(FS_ModelPass_Model);

    for (const auto& materialVecPair : data.partMap)
    {
        const auto& mtrl = materialVecPair.first;
        const auto& models = materialVecPair.second;

        impl_activate_material(context, FS_ModelPass_Model, *mtrl);

        for (const auto& model : models)
        {
            VS_ModelPass_Skelly.update<Mat4F>("matrix", model->matrix);
            VS_ModelPass_Skelly.update<Mat3F>("normMat", model->normMat);

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

    // bind all gbuffer textures for manual blending
    context.bind_Texture(textures.Gbuffer_MainDiff, 3u);
    context.bind_Texture(textures.Gbuffer_MainSurf, 4u);
    context.bind_Texture(textures.Gbuffer_MainNorm, 5u);
    context.bind_Texture(textures.Gbuffer_MainSpec, 6u);
    context.bind_Texture(textures.Depth_FullSize, 7u);

    ////// sub passes //////
    impl_render_DecalBasicPass(data.basicPass);
}

//============================================================================//

void GbufferPasses::impl_render_DecalBasicPass(const data::GbufferDecalBasicPass& data)
{
    context.use_Shader_Vert(VS_DecalPass_Decal);
    context.use_Shader_Frag(FS_DecalPass_Decal);

    for (const auto& materialVecPair : data.decalMap)
    {
        const auto& mtrl = materialVecPair.first;
        const auto& decals = materialVecPair.second;

        impl_activate_material(context, FS_DecalPass_Decal, *mtrl);

        for (const auto& decal : decals)
        {
            VS_DecalPass_Decal.update<Mat4F>("modelView", decal.matrix);
            FS_DecalPass_Decal.update<Mat4F>("invModelView", decal.invMatrix);
            FS_DecalPass_Decal.update<float>("alpha", decal.alpha);
            volumes.Unit_Cube.bind_and_draw(context);
        }
    }
}

//============================================================================//
