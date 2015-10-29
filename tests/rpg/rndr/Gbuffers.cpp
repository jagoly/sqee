#include <sqee/app/Settings.hpp>
#include <sqee/app/PreProcessor.hpp>
#include <sqee/gl/UniformBuffer.hpp>
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


void Gbuffers::render_gbuffers_base() {
    TEX_baseDiff.bind(gl::TEXTURE3);
    TEX_baseSurf.bind(gl::TEXTURE4);
    TEX_baseNorm.bind(gl::TEXTURE5);
    TEX_baseSpec.bind(gl::TEXTURE6);
    TEX_baseDpSt.bind(gl::TEXTURE7);

    sq::DEPTH_ON(); sq::DEPTH_WRITE();
    sq::STENCIL_ON(); sq::STENCIL_REPLACE();
    sq::CULLFACE_ON(); sq::CULLFACE_BACK();

    FB_baseGbuf.bind();
    gl::StencilMask(0b1111);
    sq::CLEAR_COLOR_DEPTH_STENC();
    sq::VIEWPORT(INFO_fullSize);
    gl::StencilMask(0b0001);

    gl::StencilFunc(gl::ALWAYS, 0b0001, 0b0000);
    renderer.pipeline.use_shader(FS_gbuf_base_model);
    renderer.pipeline.use_shader(VS_gbuf_base_simple);
    draw_reflects_base(true);
    draw_msimples_base(true);
    renderer.pipeline.use_shader(VS_gbuf_base_skelly);
    draw_mskellys_base(true);

    sq::CULLFACE_FRONT();
    sq::DEPTH_OFF(); sq::STENCIL_KEEP();
    gl::StencilFunc(gl::EQUAL, 0b0001, 0b0001);
    renderer.pipeline.use_shader(VS_gbuf_base_decal);
    renderer.pipeline.use_shader(FS_gbuf_base_decal);

    for (const wcoe::Decal* decal : renderer.cameraData.decalVec) {
        if (decal->texDiff) decal->texDiff->bind(gl::TEXTURE0);
        if (decal->texNorm) decal->texNorm->bind(gl::TEXTURE1);
        if (decal->texSpec) decal->texSpec->bind(gl::TEXTURE2);
        decal->ubo.bind(1u); sq::draw_volume_cube();
    }

    sq::CULLFACE_BACK();
    sq::DEPTH_ON(); sq::STENCIL_REPLACE();
    gl::StencilFunc(gl::ALWAYS, 0b0001, 0b0000);
    renderer.pipeline.use_shader(FS_gbuf_base_model);
    renderer.pipeline.use_shader(VS_gbuf_base_simple);
    draw_reflects_base(false);
    draw_msimples_base(false);
    renderer.pipeline.use_shader(VS_gbuf_base_skelly);
    draw_mskellys_base(false);

    FB_depth.attach(gl::DEPTH_ATTACHMENT, TEX_depHalf);
    FB_baseGbuf.blit(FB_depth, INFO_fullSize, INFO_halfSize, gl::DEPTH_BUFFER_BIT, gl::NEAREST);

    FB_depth.attach(gl::DEPTH_ATTACHMENT, TEX_depQter);
    FB_baseGbuf.blit(FB_depth, INFO_fullSize, INFO_qterSize, gl::DEPTH_BUFFER_BIT, gl::NEAREST);

    sq::FRONTFACE(false);
}


void Gbuffers::draw_reflects_base(bool _decals) {
    for (const wcoe::Reflector* rflct : renderer.cameraData.reflectorVec) {
        if (rflct->PROP_decals != _decals) continue;
        sq::FRONTFACE(rflct->negScale); rflct->mesh->bind_vao(); rflct->ubo.bind(1u);
        for (uint i = 0u; i < rflct->mesh->mtrlCount; ++i)
            FS_gbuf_base_model.set_vec<Vec3I>("d_n_s", rflct->skin->mtrlVec[i].glDNS),
            rflct->skin->bind_textures(i), rflct->mesh->draw_material(i);
    }
}

void Gbuffers::draw_msimples_base(bool _decals) {
    for (const wcoe::ModelSimple* model : renderer.cameraData.modelSimpleVec) {
        if (model->PROP_decals != _decals) continue;
        sq::FRONTFACE(model->negScale); model->mesh->bind_vao(); model->ubo.bind(1u);
        for (uint i = 0u; i < model->mesh->mtrlCount; ++i)
            FS_gbuf_base_model.set_vec<Vec3I>("d_n_s", model->skin->mtrlVec[i].glDNS),
            model->skin->bind_textures(i), model->mesh->draw_material(i);
    }
}

void Gbuffers::draw_mskellys_base(bool _decals) {
    for (const wcoe::ModelSkelly* model : renderer.cameraData.modelSkellyVec) {
        if (model->PROP_decals != _decals) continue;
        sq::FRONTFACE(model->negScale); model->mesh->bind_vao(); model->ubo.bind(1u);
        for (uint i = 0u; i < model->mesh->mtrlCount; ++i)
            FS_gbuf_base_model.set_vec<Vec3I>("d_n_s", model->skin->mtrlVec[i].glDNS),
            model->skin->bind_textures(i), model->mesh->draw_material(i);
    }
}


void Gbuffers::render_gbuffers_refl(const ReflectorData& _data) {
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0100);
    renderer.pipeline.use_shader(FS_gbuf_refl_model);
    renderer.pipeline.use_shader(VS_gbuf_refl_simple);
    draw_reflects_refl(_data, true);
    draw_msimples_refl(_data, true);
    renderer.pipeline.use_shader(VS_gbuf_refl_skelly);
    draw_mskellys_refl(_data, true);

    sq::CULLFACE_BACK();
    sq::DEPTH_OFF(); sq::STENCIL_KEEP();
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0101);
    renderer.pipeline.use_shader(VS_gbuf_refl_decal);
    renderer.pipeline.use_shader(FS_gbuf_refl_decal);

    for (const wcoe::Decal* decal : _data.decalVec) {
        decal->texDiff->bind(gl::TEXTURE0);
        decal->ubo.bind(1u); sq::draw_volume_cube();
    }

    sq::CULLFACE_FRONT();
    sq::DEPTH_ON(); sq::STENCIL_REPLACE();
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0100);
    renderer.pipeline.use_shader(FS_gbuf_refl_model);
    renderer.pipeline.use_shader(VS_gbuf_refl_simple);
    draw_reflects_refl(_data, false);
    draw_msimples_refl(_data, false);
    renderer.pipeline.use_shader(VS_gbuf_refl_skelly);
    draw_mskellys_refl(_data, false);

    sq::FRONTFACE(false);
}

void Gbuffers::draw_msimples_refl(const ReflectorData& _data, bool _decals) {
    for (const wcoe::ModelSimple* model : _data.modelSimpleVec) {
        if (model->PROP_decals != _decals) continue;
        sq::FRONTFACE(model->negScale); model->mesh->bind_vao(); model->ubo.bind(1u);
        for (uint i = 0u; i < model->mesh->mtrlCount; ++i)
            FS_gbuf_refl_model.set_vec<Vec3I>("d_n_s", model->skin->mtrlVec[i].glDNS),
            model->skin->bind_textures(i), model->mesh->draw_material(i);
    }
}

void Gbuffers::draw_reflects_refl(const ReflectorData& _data, bool _decals) {
    for (const wcoe::Reflector* rflct : _data.reflectorVec) {
        if (rflct->PROP_decals != _decals) continue;
        sq::FRONTFACE(rflct->negScale); rflct->mesh->bind_vao(); rflct->ubo.bind(1u);
        for (uint i = 0u; i < rflct->mesh->mtrlCount; ++i)
            FS_gbuf_refl_model.set_vec<Vec3I>("d_n_s", rflct->skin->mtrlVec[i].glDNS),
            rflct->skin->bind_textures(i), rflct->mesh->draw_material(i);
    }
}

void Gbuffers::draw_mskellys_refl(const ReflectorData& _data, bool _decals) {
    for (const wcoe::ModelSkelly* model : _data.modelSkellyVec) {
        if (model->PROP_decals != _decals) continue;
        sq::FRONTFACE(model->negScale); model->mesh->bind_vao(); model->ubo.bind(1u);
        for (uint i = 0u; i < model->mesh->mtrlCount; ++i)
            FS_gbuf_refl_model.set_vec<Vec3I>("d_n_s", model->skin->mtrlVec[i].glDNS),
            model->skin->bind_textures(i), model->mesh->draw_material(i);
    }
}


void Gbuffers::update_settings() {
    INFO_fullSize = Vec2U(renderer.settings.get<int>("app_width"),
                          renderer.settings.get<int>("app_height"));
    INFO_halfSize = INFO_fullSize / 2u;
    INFO_qterSize = INFO_fullSize / 4u;

    TEX_baseDiff.allocate_storage(INFO_fullSize, false);
    TEX_baseSurf.allocate_storage(INFO_fullSize, false);
    TEX_baseNorm.allocate_storage(INFO_fullSize, false);
    TEX_baseSpec.allocate_storage(INFO_fullSize, false);
    TEX_baseDpSt.allocate_storage(INFO_fullSize, false);
    TEX_depHalf.allocate_storage(INFO_halfSize, false);
    TEX_depQter.allocate_storage(INFO_qterSize, false);

    FB_baseGbuf.attach(gl::COLOR_ATTACHMENT0, TEX_baseDiff);
    FB_baseGbuf.attach(gl::COLOR_ATTACHMENT1, TEX_baseSurf);
    FB_baseGbuf.attach(gl::COLOR_ATTACHMENT2, TEX_baseNorm);
    FB_baseGbuf.attach(gl::COLOR_ATTACHMENT3, TEX_baseSpec);
    FB_baseGbuf.attach(gl::DEPTH_STENCIL_ATTACHMENT, TEX_baseDpSt);
}
