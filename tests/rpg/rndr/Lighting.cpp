#include <sqee/app/PreProcessor.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Drawing.hpp>

#include "../wcoe/World.hpp"
#include "../wcoe/objects/SpotLight.hpp"
#include "../wcoe/objects/PointLight.hpp"
#include "../wcoe/objects/ModelSimple.hpp"
#include "../wcoe/objects/ModelSkelly.hpp"
#include "../wcoe/objects/Reflector.hpp"
#include "../wcoe/objects/Decal.hpp"
#include "Gbuffers.hpp"
#include "Pretties.hpp"
#include "Lighting.hpp"

using namespace sqt::rndr;

Lighting::Lighting(const Renderer& _renderer) : renderer(_renderer) {

    FB_baseHdr.draw_buffers({gl::COLOR_ATTACHMENT0});

    renderer.preprocs(VS_defr_base_skybox, "deferred/base/skybox_vs");
    renderer.preprocs(VS_defr_refl_skybox, "deferred/refl/skybox_vs");
    renderer.preprocs(FS_defr_refl_ambient, "deferred/refl/ambient_fs");
    renderer.preprocs(FS_defr_skybox, "deferred/skybox_fs");
}


void Lighting::setup_render_state() {
    sq::VIEWPORT(renderer.INFO.fullSize);
    FB_baseHdr.bind(); sq::CLEAR_COLOR();
    sq::DEPTH_OFF(); sq::STENCIL_ON();
    sq::CULLFACE_OFF();
}


void Lighting::render_skybox_base() {
    renderer.pipeline.use_shader(VS_defr_base_skybox);
    renderer.pipeline.use_shader(FS_defr_skybox);
    renderer.world.skybox.tex->bind(gl::TEXTURE0);
    renderer.world.skybox.ubo->bind(1);

    sq::BLEND_OFF(); sq::STENCIL_KEEP();
    gl::StencilFunc(gl::EQUAL, 0b0001, 0b0101);
    sq::draw_screen_quad();
}


void Lighting::render_skybox_refl() {
    renderer.pipeline.use_shader(VS_defr_refl_skybox);
    renderer.pipeline.use_shader(FS_defr_skybox);
    renderer.world.skybox.tex->bind(gl::TEXTURE0);
    renderer.world.skybox.ubo->bind(1);

    sq::BLEND_OFF(); sq::STENCIL_KEEP();
    gl::StencilFunc(gl::EQUAL, 0b0011, 0b0111);
    sq::draw_screen_quad();
}


void Lighting::render_ambient_base() {
    if (renderer.INFO.ssao != 0) {
        renderer.pretties->TEX_ssao->bind(gl::TEXTURE0);
        renderer.gbuffers->TEX_depHalf->bind(gl::TEXTURE1);
    }

    renderer.world.ambient.ubo->bind(1);
    renderer.pipeline.use_shader(renderer.VS_fullscreen);
    renderer.pipeline.use_shader(FS_defr_base_ambient);

    sq::BLEND_OFF(); sq::STENCIL_KEEP();
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0101);
    sq::draw_screen_quad();
}


void Lighting::render_ambient_refl() {
    renderer.pipeline.use_shader(renderer.VS_fullscreen);
    renderer.pipeline.use_shader(FS_defr_refl_ambient);
    renderer.world.ambient.ubo->bind(1);

    sq::BLEND_OFF(); sq::STENCIL_KEEP();
    gl::StencilFunc(gl::EQUAL, 0b0111, 0b0111);
    sq::draw_screen_quad();
}


void Lighting::render_skylight_base() {
    renderer.pipeline.use_shader(renderer.VS_fullscreen);
    renderer.pipeline.use_shader(FS_defr_base_skylight);
    renderer.world.skylight.texDepthA->bind(gl::TEXTURE8);
    renderer.world.skylight.ubo->bind(1);

    sq::BLEND_ON(); sq::BLEND_ONEONE();
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0101);
    sq::STENCIL_KEEP(); sq::draw_screen_quad();
}


void Lighting::render_skylight_refl() {
    renderer.pipeline.use_shader(renderer.VS_fullscreen);
    renderer.pipeline.use_shader(FS_defr_refl_skylight);
    renderer.world.skylight.texDepthB->bind(gl::TEXTURE8);
    renderer.world.skylight.ubo->bind(1);

    sq::BLEND_ON(); sq::BLEND_ONEONE();
    gl::StencilFunc(gl::EQUAL, 0b0111, 0b0111);
    sq::STENCIL_KEEP(); sq::draw_screen_quad();
}


void Lighting::render_spotlights_base() {
    sq::BLEND_ON(); sq::BLEND_ONEONE();
    for (const wcoe::SpotLight* light : renderer.cameraData.spotLightVec) {

        light->ubo->bind(1);
        if (light->PROP_shadow) light->tex->bind(gl::TEXTURE8);

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(renderer.VS_stencil_base);
        renderer.VS_stencil_base.set_mat("matrix", light->modelMat);
        gl::StencilMask(0b1000); sq::CLEAR_STENC();
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        gl::StencilFunc(gl::EQUAL, 0b1101, 0b0101);
        sq::DEPTH_ON(); sq::DEPTH_READ();
        sq::draw_volume_cone(); sq::DEPTH_OFF();

        renderer.pipeline.use_shader(renderer.VS_fullscreen);
        renderer.pipeline.use_shader(light->PROP_shadow
            ? (light->PROP_specular ? FS_defr_base_spot_both : FS_defr_base_spot_shad)
            : (light->PROP_specular ? FS_defr_base_spot_spec : FS_defr_base_spot_none));
        gl::StencilFunc(gl::EQUAL, 0b1101, 0b1101);
        sq::STENCIL_KEEP(); sq::draw_screen_quad();
    }
}


void Lighting::render_spotlights_refl() {
//    sq::BLEND_ON(); sq::BLEND_ONEONE();
//    for (const auto& lptr : spotLightList) {
//        const wcoe::SpotLight& light = *lptr.lock();
//        if (sq::frus_in_frus(light.frus, crntRflct->frus)) continue;

//        light.ubo->bind(1); sq::Shader *shadeFS;
//        if (light.PROP_shadow == false) shadeFS = FS.defr_refl_spot_none;
//        else shadeFS = FS.defr_refl_spot_shad, light.tex->bind(gl::TEXTURE8);

//        pipeline.disable_stages(0, 0, 1);
//        pipeline.use_shader(*VS.gbuf_refl_stencil);
//        VS.gbuf_refl_stencil->set_mat("matrix", light.modelMat);
//        gl::StencilMask(0b1000); sq::CLEAR_STENC();
//        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
//        gl::StencilFunc(gl::EQUAL, 0b1111, 0b0111);
//        sq::DEPTH_ON(); sq::DEPTH_READ();
//        sq::draw_volume_cone(); sq::DEPTH_OFF();

//        pipeline.use_shader(*shadeFS);
//        pipeline.use_shader(*VS.gnrc_screen);
//        gl::StencilFunc(gl::EQUAL, 0b1111, 0b1111);
//        sq::STENCIL_KEEP(); sq::draw_screen_quad();
//    }
}


void Lighting::render_pointlights_base() {
    sq::BLEND_ON(); sq::BLEND_ONEONE();
    for (const wcoe::PointLight* light : renderer.cameraData.pointLightVec) {

        light->ubo->bind(1);
        if (light->PROP_shadow) light->tex->bind(gl::TEXTURE8);

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(renderer.VS_stencil_base);
        renderer.VS_stencil_base.set_mat("matrix", light->modelMat);
        gl::StencilMask(0b1000); sq::CLEAR_STENC();
        gl::StencilFunc(gl::EQUAL, 0b1101, 0b0101);
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        sq::DEPTH_ON(); sq::DEPTH_READ();
        sq::draw_volume_sphr(); sq::DEPTH_OFF();

        renderer.pipeline.use_shader(renderer.VS_fullscreen);
        renderer.pipeline.use_shader(light->PROP_shadow
            ? (light->PROP_specular ? FS_defr_base_point_both : FS_defr_base_point_shad)
            : (light->PROP_specular ? FS_defr_base_point_spec : FS_defr_base_point_none));
        gl::StencilFunc(gl::EQUAL, 0b1101, 0b1101);
        sq::STENCIL_KEEP(); sq::draw_screen_quad();
    }
}


void Lighting::render_pointlights_refl() {
//    sq::BLEND_ON(); sq::BLEND_ONEONE();
//    for (const auto& lptr : pointLightList) {
//        const wcoe::PointLight& light = *lptr.lock();
//        if (sq::sphr_in_frus(light.sphere, crntRflct->frus)) continue;

//        light.ubo->bind(1); sq::Shader *shadeFS;
//        if (light.PROP_shadow == false) shadeFS = FS.defr_refl_point_none;
//        else shadeFS = FS.defr_refl_point_shad, light.tex->bind(gl::TEXTURE8);

//        pipeline.disable_stages(0, 0, 1);
//        pipeline.use_shader(*VS.gbuf_refl_stencil);
//        VS.gbuf_refl_stencil->set_mat("matrix", light.modelMat);
//        gl::StencilMask(0b1000); sq::CLEAR_STENC();
//        gl::StencilFunc(gl::EQUAL, 0b1111, 0b0111);
//        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
//        sq::DEPTH_ON(); sq::DEPTH_READ();
//        sq::draw_volume_sphr(); sq::DEPTH_OFF();

//        pipeline.use_shader(*shadeFS);
//        pipeline.use_shader(*VS.gnrc_screen);
//        gl::StencilFunc(gl::EQUAL, 0b1111, 0b1111);
//        sq::STENCIL_KEEP(); sq::draw_screen_quad();
//    }
}


void Lighting::update_settings() {
    string defLightBase = "";
    if (renderer.INFO.shadlarge) defLightBase += "#define LARGE\n";
    if (renderer.INFO.shadfilter) defLightBase += "#define FILTER";
    string defLightShad = defLightBase + "\n#define SHADOW";
    string defLightSpec = defLightBase + "\n#define SPECULAR";
    string defLightBoth = defLightShad + "\n#define SPECULAR";

    renderer.preprocs(FS_defr_base_skylight, "deferred/base/skylight_fs", defLightBase);
    renderer.preprocs(FS_defr_base_spot_none, "deferred/base/spotlight_fs", defLightBase);
    renderer.preprocs(FS_defr_base_spot_shad, "deferred/base/spotlight_fs", defLightShad);
    renderer.preprocs(FS_defr_base_spot_spec, "deferred/base/spotlight_fs", defLightSpec);
    renderer.preprocs(FS_defr_base_spot_both, "deferred/base/spotlight_fs", defLightBoth);
    renderer.preprocs(FS_defr_base_point_none, "deferred/base/pointlight_fs", defLightBase);
    renderer.preprocs(FS_defr_base_point_shad, "deferred/base/pointlight_fs", defLightShad);
    renderer.preprocs(FS_defr_base_point_spec, "deferred/base/pointlight_fs", defLightSpec);
    renderer.preprocs(FS_defr_base_point_both, "deferred/base/pointlight_fs", defLightBoth);
    renderer.preprocs(FS_defr_refl_skylight, "deferred/refl/skylight_fs", defLightBase);
    renderer.preprocs(FS_defr_refl_spot_none, "deferred/refl/spotlight_fs", defLightBase);
    renderer.preprocs(FS_defr_refl_spot_shad, "deferred/refl/spotlight_fs", defLightShad);
    renderer.preprocs(FS_defr_refl_point_none, "deferred/refl/pointlight_fs", defLightBase);
    renderer.preprocs(FS_defr_refl_point_shad, "deferred/refl/pointlight_fs", defLightShad);

    int ssao = renderer.INFO.ssao;

    if (ssao == 0) renderer.preprocs(FS_defr_base_ambient, "deferred/base/ambient_fs");
    if (ssao != 0) renderer.preprocs(FS_defr_base_ambient, "deferred/base/ambient_fs", "#define SSAO");

    TEX_baseHdr.reset(new sq::Texture2D());
    TEX_baseHdr->create(gl::RGB, gl::RGB16F, 3u, false);
    TEX_baseHdr->set_preset(sq::Texture::LinearClamp());
    TEX_baseHdr->allocate_storage(renderer.INFO.fullSize);

    FB_baseHdr.attach(gl::COLOR_ATTACHMENT0, *TEX_baseHdr);
    FB_baseHdr.attach(gl::DEPTH_STENCIL_ATTACHMENT, *renderer.gbuffers->TEX_baseDpSt);
}
