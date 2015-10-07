#include <glm/gtx/string_cast.hpp>

#include <sqee/app/PreProcessor.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>

#include "../wcoe/World.hpp"
#include "Gbuffers.hpp"
#include "Lighting.hpp"
#include "Pretties.hpp"

using namespace sqt::rndr;

Pretties::Pretties(const Renderer& _renderer) : renderer(_renderer) {

    FB_simple.draw_buffers({gl::COLOR_ATTACHMENT0});

    renderer.preprocs(FS_prty_hdr_highs, "pretty/hdr/highs_fs");
    renderer.preprocs(VS_prty_shafts_shafts, "pretty/shafts/shafts_vs");
    renderer.preprocs(FS_prty_shafts_write, "pretty/shafts/write_fs");
    renderer.preprocs(FS_prty_vignette, "pretty/vignette_fs");
}


void Pretties::setup_render_state() {
    sq::DEPTH_OFF(); sq::STENCIL_OFF();
    sq::BLEND_OFF(); sq::CULLFACE_OFF();
    FB_simple.bind();
}


void Pretties::render_ssao_texture() {
    sq::VIEWPORT(renderer.INFO.halfSize);
    renderer.pipeline.use_shader(FS_prty_ssao_ssao);
    renderer.pipeline.use_shader(renderer.VS_fullscreen);
    FB_simple.attach(gl::COLOR_ATTACHMENT0, *TEX_ssao);
    renderer.gbuffers->TEX_depHalf->bind(gl::TEXTURE0);
    sq::draw_screen_quad();

    renderer.pipeline.use_shader(FS_prty_ssao_blur);
    FB_simple.attach(gl::COLOR_ATTACHMENT0, *TEX_ssaoB);
    TEX_ssao->bind(); sq::draw_screen_quad();
    FB_simple.attach(gl::COLOR_ATTACHMENT0, *TEX_ssao);
    TEX_ssaoB->bind(); sq::draw_screen_quad();
}


void Pretties::render_shafts_texture() {
    FB_simple.attach(gl::COLOR_ATTACHMENT0, *TEX_shafts);
    sq::VIEWPORT(renderer.INFO.qterSize);
    sq::CLEAR_COLOR();

    renderer.world.skylight.ubo->bind(1);
    renderer.pipeline.use_shader(VS_prty_shafts_shafts);
    renderer.pipeline.use_shader(FS_prty_shafts_shafts);
    renderer.world.skylight.texDepthA->bind(gl::TEXTURE0);
    renderer.gbuffers->TEX_depQter->bind(gl::TEXTURE1);
    sq::draw_screen_quad();

    FB_simple.attach(gl::COLOR_ATTACHMENT0, *renderer.lighting->TEX_baseHdr);
    sq::VIEWPORT(renderer.INFO.fullSize);
    sq::BLEND_ON(); sq::BLEND_ONEONE();

    renderer.gbuffers->TEX_baseDpSt->bind(gl::TEXTURE2);
    renderer.pipeline.use_shader(renderer.VS_fullscreen);
    renderer.pipeline.use_shader(FS_prty_shafts_write);
    TEX_shafts->bind(gl::TEXTURE0);
    sq::draw_screen_quad();

    sq::BLEND_OFF();
}


void Pretties::render_bloom_texture() {
    renderer.lighting->TEX_baseHdr->bind(gl::TEXTURE0);
    FB_simple.attach(gl::COLOR_ATTACHMENT0, *TEX_bloom);
    renderer.pipeline.use_shader(renderer.VS_fullscreen);
    renderer.pipeline.use_shader(FS_prty_hdr_highs);
    sq::VIEWPORT(renderer.INFO.qterSize);
    sq::draw_screen_quad();

    renderer.pipeline.use_shader(FS_prty_hdr_blurh);
    FB_simple.attach(gl::COLOR_ATTACHMENT0, *TEX_bloomB);
    TEX_bloom->bind(); sq::draw_screen_quad();
    renderer.pipeline.use_shader(FS_prty_hdr_blurv);
    FB_simple.attach(gl::COLOR_ATTACHMENT0, *TEX_bloom);
    TEX_bloomB->bind(); sq::draw_screen_quad();

    renderer.pipeline.use_shader(FS_prty_hdr_blurh);
    FB_simple.attach(gl::COLOR_ATTACHMENT0, *TEX_bloomB);
    TEX_bloom->bind(); sq::draw_screen_quad();
    renderer.pipeline.use_shader(FS_prty_hdr_blurv);
    FB_simple.attach(gl::COLOR_ATTACHMENT0, *TEX_bloom);
    TEX_bloomB->bind(); sq::draw_screen_quad();
}


void Pretties::render_hdr_tonemap() {
    sq::VIEWPORT(renderer.INFO.fullSize);
    renderer.lighting->TEX_baseHdr->bind(gl::TEXTURE0);
    if (renderer.INFO.bloom) TEX_bloom->bind(gl::TEXTURE1);
    FB_simple.attach(gl::COLOR_ATTACHMENT0, *TEX_final);
    renderer.pipeline.use_shader(renderer.VS_fullscreen);
    renderer.pipeline.use_shader(FS_prty_hdr_tones);
    sq::draw_screen_quad();
}


void Pretties::render_fsaa_screen() {
    if (renderer.INFO.fsaa) renderer.pipeline.use_shader(FS_prty_fsaa_last);
    else renderer.pipeline.use_shader(renderer.FS_passthrough);
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0u);
    TEX_final->bind(gl::TEXTURE0);
    sq::draw_screen_quad();
}


void Pretties::render_vignette() {
    renderer.pipeline.use_shader(FS_prty_vignette);
    sq::BLEND_ON(); sq::BLEND_ALPHA();
    sq::draw_screen_quad();
}


void Pretties::update_settings() {
    string defHigh = "#define HIGH";
    string defAspect = "\n#define ASPECT " + std::to_string(renderer.INFO.aspect);
    string defFPixSize = "\n#define PIXSIZE " + glm::to_string(renderer.INFO.fPixSize);
    string defHPixSize = "\n#define PIXSIZE " + glm::to_string(renderer.INFO.hPixSize);
    string defQPixSize = "\n#define PIXSIZE " + glm::to_string(renderer.INFO.qPixSize);

    int ssao = renderer.INFO.ssao;
    bool bloom = renderer.INFO.bloom;
    int shafts = renderer.INFO.shafts;
    int fsaa = renderer.INFO.fsaa;

    if (ssao != 0) renderer.preprocs(FS_prty_ssao_blur, "pretty/ssao/blur_fs", defHPixSize);
    if (ssao == 1) renderer.preprocs(FS_prty_ssao_ssao, "pretty/ssao/ssao_fs", defAspect);
    if (ssao == 2) renderer.preprocs(FS_prty_ssao_ssao, "pretty/ssao/ssao_fs", defHigh + defAspect);

    if (bloom == true) renderer.preprocs(FS_prty_hdr_blurv, "pretty/hdr/blurv_fs");
    if (bloom == true) renderer.preprocs(FS_prty_hdr_blurh, "pretty/hdr/blurh_fs", defAspect);

    if (bloom == false) renderer.preprocs(FS_prty_hdr_tones, "pretty/hdr/tones_fs");
    if (bloom == true) renderer.preprocs(FS_prty_hdr_tones, "pretty/hdr/tones_fs", "#define BLOOM");

    if (shafts == 1) renderer.preprocs(FS_prty_shafts_shafts, "pretty/shafts/shafts_fs");
    if (shafts == 2) renderer.preprocs(FS_prty_shafts_shafts, "pretty/shafts/shafts_fs", defHigh);

    if (fsaa == 1) renderer.preprocs(FS_prty_fsaa_last, "pretty/fsaa/fxaa_fs", defFPixSize);
    if (fsaa == 2) renderer.preprocs(FS_prty_fsaa_last, "pretty/fsaa/fxaa_fs", defHigh + defFPixSize);

    if (ssao != 0) {
        TEX_ssao.reset(new sq::Texture2D());
        TEX_ssaoB.reset(new sq::Texture2D());
        TEX_ssao->create(gl::RED, gl::R8, 1u, false);
        TEX_ssaoB->create(gl::RED, gl::R8, 1u, false);
        TEX_ssao->set_preset(sq::Texture::LinearClamp());
        TEX_ssaoB->set_preset(sq::Texture::LinearClamp());
        TEX_ssao->allocate_storage(renderer.INFO.halfSize);
        TEX_ssaoB->allocate_storage(renderer.INFO.halfSize);
    } else TEX_ssao.reset(), TEX_ssaoB.reset();

    if (bloom == true) {
        TEX_bloom.reset(new sq::Texture2D());
        TEX_bloomB.reset(new sq::Texture2D());
        TEX_bloom->create(gl::RGB, gl::RGB8, 3u, false);
        TEX_bloomB->create(gl::RGB, gl::RGB8, 3u, false);
        TEX_bloom->set_preset(sq::Texture::LinearClamp());
        TEX_bloomB->set_preset(sq::Texture::LinearClamp());
        TEX_bloom->allocate_storage(renderer.INFO.qterSize);
        TEX_bloomB->allocate_storage(renderer.INFO.qterSize);
    } else TEX_bloom.reset(), TEX_bloomB.reset();

    if (shafts != 0) {
        TEX_shafts.reset(new sq::Texture2D());
        TEX_shafts->create(gl::RED, gl::R8, 1u, false);
        TEX_shafts->set_preset(sq::Texture::LinearClamp());
        TEX_shafts->allocate_storage(renderer.INFO.qterSize);
    } else TEX_shafts.reset();

    TEX_final.reset(new sq::Texture2D());
    TEX_final->create(gl::RGBA, gl::RGBA8, 4u, false);
    TEX_final->set_preset(sq::Texture::LinearClamp());
    TEX_final->allocate_storage(renderer.INFO.fullSize);
}
