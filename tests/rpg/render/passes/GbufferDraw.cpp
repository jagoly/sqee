#include <sqee/gl/Context.hpp>
#include <sqee/gl/Drawing.hpp>

#include "../../Options.hpp"
#include "GbufferDraw.hpp"

using Context = sq::Context;
using namespace sqt::render;


GbufferPasses::GbufferPasses(const SharedStuff& _stuff) : SharedStuff(_stuff) {

    FB_MainGbuffer.draw_buffers({
        gl::COLOR_ATTACHMENT0, gl::COLOR_ATTACHMENT1,
        gl::COLOR_ATTACHMENT2, gl::COLOR_ATTACHMENT3
    });

    VS_ModelPass_Simple.add_uniform("matrix"); // Mat4F
    VS_ModelPass_Simple.add_uniform("normMat"); // Mat3F
    VS_ModelPass_Skelly.add_uniform("matrix"); // Mat4F
    VS_ModelPass_Skelly.add_uniform("normMat"); // Mat3F
    FS_ModelPass_Model.add_uniform("d_n_s"); // Vec3I

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


void GbufferPasses::update_options() {
    //static const auto& options = Options::get();

    FB_MainGbuffer.attach(gl::COLOR_ATTACHMENT0, textures.Gbuffer_MainDiff);
    FB_MainGbuffer.attach(gl::COLOR_ATTACHMENT1, textures.Gbuffer_MainSurf);
    FB_MainGbuffer.attach(gl::COLOR_ATTACHMENT2, textures.Gbuffer_MainNorm);
    FB_MainGbuffer.attach(gl::COLOR_ATTACHMENT3, textures.Gbuffer_MainSpec);
    FB_MainGbuffer.attach(gl::DEPTH_STENCIL_ATTACHMENT, textures.Depth_FullSize);
}


void GbufferPasses::render(const data::GbufferPasses& _data) {

    static auto& context = Context::get();
    static const auto& options = Options::get();

    // bind and clear framebuffer
    context.set_ViewPort(options.Window_Size);
    context.bind_FrameBuffer(FB_MainGbuffer);
    context.clear_Colour(Vec4F(0.f));

    ////// sub passes //////
    impl_render_ModelPasses(_data.modelPasses);
    impl_render_DecalPasses(_data.decalPasses);
}


void GbufferPasses::impl_render_ModelPasses(const data::GbufferModelPasses& _data) {

    static auto& context = Context::get();

    // use back face culling for all models
    context.set_state(Context::Cull_Face::Back);

    // test if model is visible in the depth buffer
    context.set_state(Context::Depth_Compare::Equal);
    context.set_state(Context::Depth_Test::Keep);

    // disable stencil test and blending
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Disable);

    ////// sub passes //////
    impl_render_ModelSimplePass(_data.simplePass);
    impl_render_ModelSkellyPass(_data.skellyPass);

    // revert to standard depth comparison
    context.set_state(Context::Depth_Compare::LessEqual);

    // revert modified state
    sq::FRONTFACE(false);
}

void GbufferPasses::impl_render_ModelSimplePass(const data::GbufferModelSimplePass& _data) {

    shaders.pipeline.use_shader(VS_ModelPass_Simple);
    shaders.pipeline.use_shader(FS_ModelPass_Model);

    for (const auto& materialVecPair : _data.partMap) {

        const Material& mtrl = *(materialVecPair.first);
        Vec3I d_n_s{mtrl.diff.check(), mtrl.norm.check(), mtrl.spec.check()};
        FS_ModelPass_Model.update<Vec3I>("d_n_s", d_n_s); mtrl.bind_dns();

        for (const auto& model : materialVecPair.second) {
            VS_ModelPass_Simple.update<Mat4F>("matrix", model->matrix);
            VS_ModelPass_Simple.update<Mat3F>("normMat", model->normMat);
            sq::FRONTFACE(model->mirror); model->mesh.bind_vao();
            model->mesh.draw_material(model.index);
    }}
}

void GbufferPasses::impl_render_ModelSkellyPass(const data::GbufferModelSkellyPass& _data) {

    static auto& context = Context::get();

    shaders.pipeline.use_shader(VS_ModelPass_Skelly);
    shaders.pipeline.use_shader(FS_ModelPass_Model);

    for (const auto& materialVecPair : _data.partMap) {

        const Material& mtrl = *(materialVecPair.first);
        Vec3I d_n_s{mtrl.diff.check(), mtrl.norm.check(), mtrl.spec.check()};
        FS_ModelPass_Model.update<Vec3I>("d_n_s", d_n_s); mtrl.bind_dns();

        for (const auto& model : materialVecPair.second) {
            VS_ModelPass_Skelly.update<Mat4F>("matrix", model->matrix);
            VS_ModelPass_Skelly.update<Mat3F>("normMat", model->normMat);
            sq::FRONTFACE(model->mirror); model->mesh.bind_vao();
            context.bind_UniformBuffer(model->ubo, 1u);
            model->mesh.draw_material(model.index);
    }}
}


void GbufferPasses::impl_render_DecalPasses(const data::GbufferDecalPasses& _data) {

    static auto& context = Context::get();

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
    impl_render_DecalBasicPass(_data.basicPass);
}

void GbufferPasses::impl_render_DecalBasicPass(const data::GbufferDecalBasicPass& _data) {

    shaders.pipeline.use_shader(VS_DecalPass_Decal);
    shaders.pipeline.use_shader(FS_DecalPass_Decal);

    for (const auto& materialVecPair : _data.decalMap) {

        const Material& mtrl = *(materialVecPair.first);
        Vec3I d_n_s{mtrl.diff.check(), mtrl.norm.check(), mtrl.spec.check()};
        FS_DecalPass_Decal.update<Vec3I>("d_n_s", d_n_s); mtrl.bind_dns();

        for (const auto& decal : materialVecPair.second) {
            VS_DecalPass_Decal.update<Mat4F>("modelView", decal.matrix);
            FS_DecalPass_Decal.update<Mat4F>("invModelView", decal.invMatrix);
            FS_DecalPass_Decal.update<float>("alpha", decal.alpha);
            volumes.Unit_Cube.bind_and_draw();
    }}
}
