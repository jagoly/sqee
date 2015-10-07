#include <sqee/app/PreProcessor.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>

#include "../wcoe/World.hpp"
#include "../wcoe/objects/ModelSimple.hpp"
#include "../wcoe/objects/ModelSkelly.hpp"
#include "../wcoe/objects/Reflector.hpp"
#include "../wcoe/objects/Decal.hpp"
#include "Gbuffers.hpp"

using namespace sqt::rndr;

Gbuffers::Gbuffers(const Renderer& _renderer) : renderer(_renderer) {

    FB_baseGbuf.draw_buffers({gl::COLOR_ATTACHMENT0, gl::COLOR_ATTACHMENT1,
                              gl::COLOR_ATTACHMENT2, gl::COLOR_ATTACHMENT3});

    FS_gbuf_base_model.add_uniform("d_n_s"); // ivec3
    FS_gbuf_refl_model.add_uniform("d_n_s"); // ivec3

    renderer.preprocs(VS_gbuf_base_simple, "gbuffer/base/simple_vs");
    renderer.preprocs(VS_gbuf_base_skelly, "gbuffer/base/skelly_vs");
    renderer.preprocs(VS_gbuf_base_decal, "gbuffer/base/decal_vs");
    renderer.preprocs(FS_gbuf_base_model, "gbuffer/base/model_fs");
    renderer.preprocs(FS_gbuf_base_decal, "gbuffer/base/decal_fs");

    renderer.preprocs(VS_gbuf_refl_simple, "gbuffer/refl/simple_vs");
    renderer.preprocs(VS_gbuf_refl_skelly, "gbuffer/refl/skelly_vs");
    renderer.preprocs(VS_gbuf_refl_decal, "gbuffer/refl/decal_vs");
    renderer.preprocs(FS_gbuf_refl_model, "gbuffer/refl/model_fs");
    renderer.preprocs(FS_gbuf_refl_decal, "gbuffer/refl/decal_fs");
}


void Gbuffers::setup_render_state() {
    sq::DEPTH_ON();
    sq::STENCIL_ON();
    sq::CULLFACE_ON();
    sq::DEPTH_WRITE();
    sq::CULLFACE_BACK();
    sq::STENCIL_REPLACE();

    sq::VIEWPORT(renderer.INFO.fullSize);

    FB_baseGbuf.bind();
    gl::StencilMask(0b1111);
    gl::ClearStencil(0b0001);
    sq::CLEAR_COLOR_DEPTH_STENC();
    gl::ClearStencil(0b0000);
    gl::StencilMask(0b0100);
}


void Gbuffers::bind_textures_base() {
    TEX_baseDiff->bind(gl::TEXTURE3);
    TEX_baseSurf->bind(gl::TEXTURE4);
    TEX_baseNorm->bind(gl::TEXTURE5);
    TEX_baseSpec->bind(gl::TEXTURE6);
    TEX_baseDpSt->bind(gl::TEXTURE7);
}


void Gbuffers::render_msimples_base(bool _decals) {
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0001);
    renderer.pipeline.use_shader(VS_gbuf_base_simple);
    renderer.pipeline.use_shader(FS_gbuf_base_model);

    for (const wcoe::ModelSimple* model : renderer.cameraData.modelSimpleVec) {
        if (model->PROP_decals != _decals) continue;
        sq::FRONTFACE(model->negScale); model->mesh->bind_vao(); model->ubo->bind(1);
        for (uint i = 0u; i < model->mesh->mtrlCount; ++i)
            FS_gbuf_base_model.set_vec<ivec3>("d_n_s", model->skin->mtrlVec[i].glDNS),
            model->skin->bind_textures(i), model->mesh->draw_material(i);
    } sq::FRONTFACE(false);
}


void Gbuffers::render_msimples_refl(bool _decals) {
//    gl::StencilMask(0b0100);
//    sq::DEPTH_ON(); sq::DEPTH_WRITE();
//    sq::CULLFACE_ON(); sq::CULLFACE_FRONT();
//    sq::STENCIL_ON(); sq::STENCIL_REPLACE();
//    gl::StencilFunc(gl::EQUAL, 0b0111, 0b0011);
//    pipeline.use_shader(*VS.gbuf_refl_simple);
//    pipeline.use_shader(*FS.gbuf_refl_model);

//    for (const auto& mptr : modelSimpleList) {
//        const wcoe::ModelSimple& model = *mptr.lock();
//        if (model.PROP_render == false) continue;
//        if (model.PROP_decals != _decals) continue;
//        if (sq::bbox_in_frus(model.bbox, crntRflct->frus)) continue;
//        sq::FRONTFACE(model.negScale); model.ubo->bind(1); model.mesh->bind_vao();
//        for (uint i = 0u; i < model.mesh->mtrlCount; i++)
//            FS.gbuf_refl_model->set_vec<ivec3>("d_n_s", model.skin->mtrlVec[i].glDNS),
//            model.skin->bind_textures(i), model.mesh->draw_ibo(i);
//    } gl::FrontFace(gl::CCW);
}


void Gbuffers::render_mskellys_base(bool _decals) {
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0001);
    renderer.pipeline.use_shader(VS_gbuf_base_skelly);
    renderer.pipeline.use_shader(FS_gbuf_base_model);

    for (const wcoe::ModelSkelly* model : renderer.cameraData.modelSkellyVec) {
        if (model->PROP_decals != _decals) continue;
        sq::FRONTFACE(model->negScale); model->mesh->bind_vao(); model->ubo->bind(1);
        for (uint i = 0u; i < model->mesh->mtrlCount; ++i)
            FS_gbuf_base_model.set_vec<ivec3>("d_n_s", model->skin->mtrlVec[i].glDNS),
            model->skin->bind_textures(i), model->mesh->draw_material(i);
    } sq::FRONTFACE(false);
}


void Gbuffers::render_mskellys_refl(bool _decals) {
//    gl::StencilMask(0b0100);
//    sq::DEPTH_ON(); sq::DEPTH_WRITE();
//    sq::CULLFACE_ON(); sq::CULLFACE_FRONT();
//    sq::STENCIL_ON(); sq::STENCIL_REPLACE();
//    gl::StencilFunc(gl::EQUAL, 0b0111, 0b0011);
//    pipeline.use_shader(*VS.gbuf_refl_skelly);
//    pipeline.use_shader(*FS.gbuf_refl_model);

//    for (const auto& mptr : modelSkellyList) {
//        const wcoe::ModelSkelly& model = *mptr.lock();
//        if (model.PROP_render == false) continue;
//        if (model.PROP_decals != _decals) continue;
//        if (sq::sphr_in_frus(model.sphere, crntRflct->frus)) continue;
//        sq::FRONTFACE(model.negScale); model.ubo->bind(1); model.mesh->bind_vao();
//        for (uint i = 0u; i < model.mesh->mtrlCount; i++)
//            FS.gbuf_refl_model->set_vec<ivec3>("d_n_s", model.skin->mtrlVec[i].glDNS),
//            model.skin->bind_textures(i), model.mesh->draw_ibo(i);
//    } sq::FRONTFACE(false);
}


void Gbuffers::render_reflects_base(bool _decals) {
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0001);
    renderer.pipeline.use_shader(VS_gbuf_base_simple);
    renderer.pipeline.use_shader(FS_gbuf_base_model);

    for (const wcoe::Reflector* rflct : renderer.cameraData.reflectorVec) {
        if (rflct->PROP_decals != _decals) continue;
        sq::FRONTFACE(rflct->negScale); rflct->mesh->bind_vao(); rflct->ubo->bind(1);
        for (uint i = 0u; i < rflct->mesh->mtrlCount; ++i)
            FS_gbuf_base_model.set_vec<ivec3>("d_n_s", rflct->skin->mtrlVec[i].glDNS),
            rflct->skin->bind_textures(i), rflct->mesh->draw_material(i);
    } sq::FRONTFACE(false);
}


void Gbuffers::render_reflects_refl(bool _decals) {
//    gl::StencilMask(0b0100);
//    sq::DEPTH_ON(); sq::DEPTH_WRITE();
//    sq::CULLFACE_ON(); sq::CULLFACE_FRONT();
//    sq::STENCIL_ON(); sq::STENCIL_REPLACE();
//    gl::StencilFunc(gl::EQUAL, 0b0111, 0b0011);
//    pipeline.use_shader(*VS.gbuf_refl_simple);
//    pipeline.use_shader(*FS.gbuf_refl_model);

//    for (const auto& rptr : reflectorList) {
//        const wcoe::Reflector& rflct = *rptr.lock();
//        if (rflct.PROP_decals != _decals) continue;
//        if (sq::bbox_in_frus(rflct.bbox, crntRflct->frus)) continue;
//        sq::FRONTFACE(rflct.negScale); rflct.ubo->bind(1); rflct.mesh->bind_vao();
//        for (uint i = 0u; i < rflct.skin->mtrlVec.size(); i++)
//            FS.gbuf_refl_model->set_vec<ivec3>("d_n_s", rflct.skin->mtrlVec[i].glDNS),
//            rflct.skin->bind_textures(i), rflct.mesh->draw_ibo(i);
//    } sq::FRONTFACE(false);
}


void Gbuffers::render_decals_base() {
    sq::CULLFACE_FRONT();
    sq::DEPTH_OFF(); sq::STENCIL_KEEP();
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0101);
    renderer.pipeline.use_shader(VS_gbuf_base_decal);
    renderer.pipeline.use_shader(FS_gbuf_base_decal);

    for (const wcoe::Decal* decal : renderer.cameraData.decalVec) {
        if (decal->texDiff) decal->texDiff->bind(gl::TEXTURE0);
        if (decal->texNorm) decal->texNorm->bind(gl::TEXTURE1);
        if (decal->texSpec) decal->texSpec->bind(gl::TEXTURE2);
        decal->ubo->bind(1u); sq::draw_volume_cube();
    }

    sq::DEPTH_ON(); sq::CULLFACE_BACK();
    sq::STENCIL_REPLACE();
}


void Gbuffers::render_decals_refl() {
//    sq::DEPTH_OFF();
//    sq::STENCIL_ON(); sq::STENCIL_KEEP();
//    sq::CULLFACE_ON(); sq::CULLFACE_BACK();
//    gl::StencilFunc(gl::EQUAL, 0b0111, 0b0111);
//    pipeline.use_shader(*VS.gbuf_refl_decal);
//    pipeline.use_shader(*FS.gbuf_refl_decal);

//    for (const auto& dptr : decalList) {
//        const wcoe::Decal& decal = *dptr.lock();
//        if (sq::bbox_in_frus(decal.bbox, crntRflct->frus)) continue;
//        if (decal.texDiff) decal.texDiff->bind(gl::TEXTURE0);
//        else continue; decal.ubo->bind(1); sq::draw_volume_cube();
//    }
}


void Gbuffers::render_downscaled_depth() {
    uvec2 fullSize = renderer.INFO.fullSize;

    FB_depth.attach(gl::DEPTH_ATTACHMENT, *TEX_depHalf);
    FB_baseGbuf.blit(FB_depth, fullSize, renderer.INFO.halfSize, gl::DEPTH_BUFFER_BIT, gl::NEAREST);

    FB_depth.attach(gl::DEPTH_ATTACHMENT, *TEX_depQter);
    FB_baseGbuf.blit(FB_depth, fullSize, renderer.INFO.qterSize, gl::DEPTH_BUFFER_BIT, gl::NEAREST);
}


void Gbuffers::update_settings() {
    TEX_baseDiff.reset(new sq::Texture2D());
    TEX_baseSurf.reset(new sq::Texture2D());
    TEX_baseNorm.reset(new sq::Texture2D());
    TEX_baseSpec.reset(new sq::Texture2D());

    TEX_baseDpSt.reset(new sq::Texture2D());
    TEX_depHalf.reset(new sq::Texture2D());
    TEX_depQter.reset(new sq::Texture2D());

    TEX_baseDiff->create(gl::RGB, gl::RGB8, 3u, false);
    TEX_baseSurf->create(gl::RGB, gl::RGB10, 3u, false);
    TEX_baseNorm->create(gl::RGB, gl::RGB10, 3u, false);
    TEX_baseSpec->create(gl::RGB, gl::RGB8, 3u, false);

    TEX_baseDpSt->create(gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, 1u, false);
    TEX_depHalf->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT24, 1u, false);
    TEX_depQter->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT24, 1u, false);

    TEX_baseDiff->allocate_storage(renderer.INFO.fullSize);
    TEX_baseSurf->allocate_storage(renderer.INFO.fullSize);
    TEX_baseNorm->allocate_storage(renderer.INFO.fullSize);
    TEX_baseSpec->allocate_storage(renderer.INFO.fullSize);

    TEX_baseDpSt->allocate_storage(renderer.INFO.fullSize);
    TEX_depHalf->allocate_storage(renderer.INFO.halfSize);
    TEX_depQter->allocate_storage(renderer.INFO.qterSize);

    TEX_baseDiff->set_preset(sq::Texture::LinearClamp());
    TEX_baseSurf->set_preset(sq::Texture::LinearClamp());
    TEX_baseNorm->set_preset(sq::Texture::LinearClamp());
    TEX_baseSpec->set_preset(sq::Texture::LinearClamp());

    TEX_baseDpSt->set_preset(sq::Texture::LinearClamp());
    TEX_depHalf->set_preset(sq::Texture::LinearClamp());
    TEX_depQter->set_preset(sq::Texture::LinearClamp());

    FB_baseGbuf.attach(gl::COLOR_ATTACHMENT0, *TEX_baseDiff);
    FB_baseGbuf.attach(gl::COLOR_ATTACHMENT1, *TEX_baseSurf);
    FB_baseGbuf.attach(gl::COLOR_ATTACHMENT2, *TEX_baseNorm);
    FB_baseGbuf.attach(gl::COLOR_ATTACHMENT3, *TEX_baseSpec);
    FB_baseGbuf.attach(gl::DEPTH_STENCIL_ATTACHMENT, *TEX_baseDpSt);
}
