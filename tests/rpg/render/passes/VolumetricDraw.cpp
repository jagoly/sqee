#include <sqee/gl/Context.hpp>
#include <sqee/gl/Drawing.hpp>

#include "../../Options.hpp"
#include "VolumetricDraw.hpp"

using Context = sq::Context;
using namespace sqt::render;


VolumetricPasses::VolumetricPasses(const SharedStuff& _stuff) : SharedStuff(_stuff) {

    FB_Shafts.draw_buffers({gl::COLOR_ATTACHMENT0});

    VS_Shafts_Stencil.add_uniform("matrix"); // Mat4F

    shaders.preprocs(VS_Shafts_Stencil, "volumetric/Shafts/Stencil_vs");
    shaders.preprocs(VS_Shafts_Light, "volumetric/Shafts/Light_vs");
}


void VolumetricPasses::update_options() {

    static const auto& options = Options::get();

    if (options.Shafts_Quality) shaders.preprocs(FS_Shafts_LightCasc, "volumetric/Shafts/LightCasc_fs");
    if (options.Shafts_Quality) shaders.preprocs(FS_Shafts_LightOrtho, "volumetric/Shafts/LightOrtho_fs");
    if (options.Shafts_Quality) shaders.preprocs(FS_Shafts_LightPoint, "volumetric/Shafts/LightPoint_fs");
    if (options.Shafts_Quality) shaders.preprocs(FS_Shafts_LightSpot, "volumetric/Shafts/LightSpot_fs");

    // delete or allocate shafts depth stencil texture
    if (options.Shafts_Quality == 0u) TEX_Shafts_Depth.delete_object();
    else TEX_Shafts_Depth.allocate_storage(options.Window_Size/2u, false);

    // re-attach textures to shafts framebuffer
    if (options.Shafts_Quality) FB_Shafts.attach(gl::COLOR_ATTACHMENT0, textures.Volumetric_Shafts);
    if (options.Shafts_Quality) FB_Shafts.attach(gl::DEPTH_STENCIL_ATTACHMENT, TEX_Shafts_Depth);

    FB_HalfDepthBlit.attach(gl::DEPTH_STENCIL_ATTACHMENT, textures.Depth_HalfSize);
}


void VolumetricPasses::render(const data::VolumetricPasses& _data) {

    static auto& context = Context::get();
    static const auto& options = Options::get();

    if (options.Shafts_Quality == 0u) return;

    // bind and clear framebuffer
    context.set_ViewPort(options.Window_Size / 2u);
    context.bind_FrameBuffer(FB_Shafts);
    context.clear_Colour(Vec4F(0.f));

    // set blending mode for light accumulation
    context.set_state(Context::Blend_Mode::Accumulate);


    context.bind_Texture(TEX_Shafts_Depth, 7u);

    // render cascade light shafts pass without stencil
    if (_data.skylightPass != nullptr) impl_render_SkyLightPass(*_data.skylightPass);

    context.bind_Texture(TEX_Shafts_Depth, 7u);

    // render shafts of all other types of lights using stencil volumes
//    for (const auto& light : _data.orthoPassVec) impl_render_StencilPass(light, volumes.Light_Ortho, FS_Shafts_LightOrtho);
    for (const auto& light : _data.pointPassVec) impl_render_StencilPass(light, volumes.Light_Point, FS_Shafts_LightPoint);
    for (const auto& light : _data.spotPassVec) impl_render_StencilPass(light, volumes.Light_Spot, FS_Shafts_LightSpot);
}


void VolumetricPasses::impl_render_SkyLightPass(const data::VolumetricSkyLightPass& _light) {

    static auto& context = Context::get();

    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);

    context.bind_UniformBuffer(_light.ubo, 1u);
    context.bind_Texture(textures.Depth_HalfSize, 7u);
    context.bind_Texture(_light.tex, 8u);

    shaders.pipeline.use_shader(VS_Shafts_Light);
    shaders.pipeline.use_shader(FS_Shafts_LightCasc);
    sq::draw_screen_quad();
}

void VolumetricPasses::impl_render_StencilPass(const data::VolumetricStencilPass& _light,
                                               const sq::Volume& _volume, const sq::Shader& _shader) {

    static auto& context = Context::get();
    static const auto& options = Options::get();

    FB_HalfDepthBlit.blit(FB_Shafts, options.Window_Size / 2u, gl::DEPTH_BUFFER_BIT);


    shaders.pipeline.use_shader(VS_Shafts_Stencil);
    shaders.pipeline.disable_stages(0, 0, true);

    VS_Shafts_Stencil.update<Mat4F>("matrix", _light.matrix);


    if (options.Vignette_Enable && _light.stencil == true) {
        context.set_state(Context::Cull_Face::Back);
        context.set_state(Context::Depth_Test::Keep);
        context.set_state(Context::Stencil_Test::Replace);
        context.set_Stencil_Params(gl::ALWAYS, 2, 0, 2);

        context.clear_Stencil(0, 255);

        _volume.bind_and_draw();

        context.set_state(Context::Stencil_Test::Keep);
        context.set_Stencil_Params(gl::EQUAL, 2, 2, 0);
    }
    else { context.set_state(Context::Stencil_Test::Disable); }


    context.set_state(Context::Cull_Face::Front);
    context.set_state(Context::Depth_Test::Replace);

    _volume.bind_and_draw();


    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);

    context.bind_UniformBuffer(_light.ubo, 1u);
    context.bind_Texture(_light.tex, 8u);

    shaders.pipeline.use_shader(VS_Shafts_Light);
    shaders.pipeline.use_shader(_shader);
    sq::draw_screen_quad();
}
