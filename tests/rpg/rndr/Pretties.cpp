#include <sqee/app/Settings.hpp>
#include <sqee/app/PreProcessor.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>
#include <sqee/misc/StringCast.hpp>

#include "../wcoe/World.hpp"
#include "../wcoe/SkyLight.hpp"
#include "Gbuffers.hpp"
#include "Lighting.hpp"
#include "Pretties.hpp"

using namespace sqt;

Renderer::Pretties::Pretties(const Renderer& _renderer) : renderer(_renderer) {
    FB_simple.draw_buffers({gl::COLOR_ATTACHMENT0});

    renderer.preprocs(FS_prty_hdr_highs, "pretty/hdr/highs_fs");
    renderer.preprocs(VS_prty_shafts_shafts, "pretty/shafts/shafts_vs");
    renderer.preprocs(FS_prty_shafts_write, "pretty/shafts/write_fs");
    renderer.preprocs(FS_prty_vignette, "pretty/vignette_fs");
}


void Renderer::Pretties::render_post_gbuffers() {
    sq::DEPTH_OFF(); sq::STENCIL_OFF();
    sq::BLEND_OFF(); sq::CULLFACE_OFF();
    FB_simple.bind();

    if (INFO_ssao > 0) {
        sq::VIEWPORT(INFO_halfSize);
        renderer.pipeline.use_shader(FS_prty_ssao_ssao);
        renderer.pipeline.use_shader(renderer.VS_fullscreen);
        FB_simple.attach(gl::COLOR_ATTACHMENT0, TEX_ssao);
        renderer.gbuffers->TEX_depHalf.bind(gl::TEXTURE0);
        sq::draw_screen_quad();

        renderer.pipeline.use_shader(FS_prty_ssao_blur);
        FB_simple.attach(gl::COLOR_ATTACHMENT0, TEX_ssaoB);
        TEX_ssao.bind(); sq::draw_screen_quad();
        FB_simple.attach(gl::COLOR_ATTACHMENT0, TEX_ssao);
        TEX_ssaoB.bind(); sq::draw_screen_quad();
    }
}


void Renderer::Pretties::render_post_lighting() {
    sq::DEPTH_OFF(); sq::CULLFACE_OFF();
    sq::BLEND_OFF(); sq::STENCIL_OFF();
    sq::VIEWPORT(INFO_qterSize);
    FB_simple.bind();

    if (INFO_shafts > 0) {
        FB_simple.attach(gl::COLOR_ATTACHMENT0, TEX_shafts);
        sq::CLEAR_COLOR();

        renderer.world.skylight->texA.bind(gl::TEXTURE0);
        renderer.gbuffers->TEX_depQter.bind(gl::TEXTURE1);
        renderer.pipeline.use_shader(VS_prty_shafts_shafts);
        renderer.pipeline.use_shader(FS_prty_shafts_shafts);
        renderer.world.skylight->ubo.bind(1u);
        sq::draw_screen_quad();
    }

    if (INFO_bloom == true) {
        renderer.lighting->TEX_baseHdr.bind(gl::TEXTURE0);
        FB_simple.attach(gl::COLOR_ATTACHMENT0, TEX_bloom);
        renderer.pipeline.use_shader(renderer.VS_fullscreen);
        renderer.pipeline.use_shader(FS_prty_hdr_highs);
        sq::draw_screen_quad();

        renderer.pipeline.use_shader(FS_prty_hdr_blurh);
        FB_simple.attach(gl::COLOR_ATTACHMENT0, TEX_bloomB);
        TEX_bloom.bind(); sq::draw_screen_quad();
        renderer.pipeline.use_shader(FS_prty_hdr_blurv);
        FB_simple.attach(gl::COLOR_ATTACHMENT0, TEX_bloom);
        TEX_bloomB.bind(); sq::draw_screen_quad();

        renderer.pipeline.use_shader(FS_prty_hdr_blurh);
        FB_simple.attach(gl::COLOR_ATTACHMENT0, TEX_bloomB);
        TEX_bloom.bind(); sq::draw_screen_quad();
        renderer.pipeline.use_shader(FS_prty_hdr_blurv);
        FB_simple.attach(gl::COLOR_ATTACHMENT0, TEX_bloom);
        TEX_bloomB.bind(); sq::draw_screen_quad();
    }
}


void Renderer::Pretties::render_final_screen() {
    renderer.pipeline.use_shader(renderer.VS_fullscreen);
    sq::DEPTH_OFF(); sq::BLEND_OFF(); sq::STENCIL_OFF();
    sq::CULLFACE_OFF(); sq::VIEWPORT(INFO_fullSize);

    renderer.lighting->TEX_baseHdr.bind(gl::TEXTURE0);
    if (INFO_shafts > 0) TEX_shafts.bind(gl::TEXTURE1),
        renderer.gbuffers->TEX_depQter.bind(gl::TEXTURE2),
        renderer.gbuffers->TEX_baseDpSt.bind(gl::TEXTURE3);
    if (INFO_bloom == true) TEX_bloom.bind(gl::TEXTURE4);
    FB_simple.attach(gl::COLOR_ATTACHMENT0, TEX_final);
    renderer.pipeline.use_shader(FS_prty_hdr_tones);
    FB_simple.bind(); sq::draw_screen_quad();

    gl::BindFramebuffer(gl::FRAMEBUFFER, 0u);
    if (INFO_fsaa > 0) renderer.pipeline.use_shader(FS_prty_fsaa_last);
    else renderer.pipeline.use_shader(renderer.FS_passthrough);
    TEX_final.bind(gl::TEXTURE0); sq::draw_screen_quad();

    if (INFO_vignette == true) {
        sq::BLEND_ON(); sq::BLEND_ALPHA();
        renderer.pipeline.use_shader(FS_prty_vignette);
        sq::draw_screen_quad();
    }
}


void Renderer::Pretties::update_settings() {
    INFO_fullSize = Vec2U(renderer.settings.get<int>("app_width"),
                          renderer.settings.get<int>("app_height"));
    INFO_halfSize = INFO_fullSize / 2u;
    INFO_qterSize = INFO_fullSize / 4u;

    INFO_ssao = renderer.settings.get<int>("rpg_ssao");
    INFO_shafts = renderer.settings.get<int>("rpg_shafts");
    INFO_vignette = renderer.settings.get<bool>("rpg_vignette");
    INFO_bloom = renderer.settings.get<bool>("rpg_bloom");
    INFO_fsaa = renderer.settings.get<int>("rpg_fsaa");

    float aspect = float(INFO_fullSize.x) / float(INFO_fullSize.y);
    Vec2F fPixSize = Vec2F(1.f, 1.f) / Vec2F(INFO_fullSize);
    Vec2F hPixSize = Vec2F(1.f, 1.f) / Vec2F(INFO_halfSize);

    string defAspect = "\n#define ASPECT " + std::to_string(aspect);
    string defFPixSize = "\n#define PIXSIZE " + sq::glsl_string(fPixSize);
    string defHPixSize = "\n#define PIXSIZE " + sq::glsl_string(hPixSize);

    if (INFO_ssao > 0) {
        renderer.preprocs(FS_prty_ssao_blur, "pretty/ssao/blur_fs", defHPixSize);
        if (INFO_ssao == 1) renderer.preprocs(FS_prty_ssao_ssao, "pretty/ssao/ssao_fs", defAspect);
        else renderer.preprocs(FS_prty_ssao_ssao, "pretty/ssao/ssao_fs", "#define HIGH" + defAspect);
    }

    if (INFO_shafts > 0) {
        if (INFO_shafts == 1) renderer.preprocs(FS_prty_shafts_shafts, "pretty/shafts/shafts_fs");
        else renderer.preprocs(FS_prty_shafts_shafts, "pretty/shafts/shafts_fs", "#define HIGH");
    }

    if (INFO_bloom == true) {
        renderer.preprocs(FS_prty_hdr_blurv, "pretty/hdr/blurv_fs");
        renderer.preprocs(FS_prty_hdr_blurh, "pretty/hdr/blurh_fs", defAspect);
    }

    if (INFO_shafts > 0 && INFO_bloom == true)
        renderer.preprocs(FS_prty_hdr_tones, "pretty/hdr/tones_fs", "#define SHAFTS\n#define BLOOM");
    else if (INFO_shafts) renderer.preprocs(FS_prty_hdr_tones, "pretty/hdr/tones_fs", "#define SHAFTS");
    else if (INFO_bloom) renderer.preprocs(FS_prty_hdr_tones, "pretty/hdr/tones_fs", "#define BLOOM");
    else renderer.preprocs(FS_prty_hdr_tones, "pretty/hdr/tones_fs");

    if (INFO_fsaa > 0) {
        if (INFO_fsaa == 1) renderer.preprocs(FS_prty_fsaa_last, "pretty/fsaa/fxaa_fs", defFPixSize);
        else renderer.preprocs(FS_prty_fsaa_last, "pretty/fsaa/fxaa_fs", "#define HIGH" + defFPixSize);
    }

    if (INFO_ssao > 0) {
        TEX_ssao.allocate_storage(INFO_halfSize, false);
        TEX_ssaoB.allocate_storage(INFO_halfSize, false);
    } else TEX_ssao.delete_object(), TEX_ssaoB.delete_object();

    if (INFO_shafts > 0) {
        TEX_shafts.allocate_storage(INFO_qterSize, false);
    } else TEX_shafts.delete_object();

    if (INFO_bloom == true) {
        TEX_bloom.allocate_storage(INFO_qterSize, false);
        TEX_bloomB.allocate_storage(INFO_qterSize, false);
    } else TEX_bloom.delete_object(), TEX_bloomB.delete_object();

    TEX_final.allocate_storage(INFO_fullSize, false);
}
