#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>

#include "../components/Transform.hpp"
#include "../components/Model.hpp"
#include "../components/Decal.hpp"

#include "Gbuffers.hpp"

using namespace sqt;

Renderer::Gbuffers::Gbuffers(const Renderer& _renderer)
    : renderer(_renderer), options(renderer.options) {

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


void Renderer::Gbuffers::render_gbuffers_base() {
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
    draw_msimples_base(true);
    renderer.pipeline.use_shader(VS_gbuf_base_skelly);
    draw_mskellys_base(true);

    sq::CULLFACE_FRONT();
    sq::DEPTH_OFF(); sq::STENCIL_KEEP();
    gl::StencilFunc(gl::EQUAL, 0b0001, 0b0001);
    renderer.pipeline.use_shader(VS_gbuf_base_decal);
    renderer.pipeline.use_shader(FS_gbuf_base_decal);


    // TODO: Add shader for complete decals

    for (const DecalComponent* decalC : renderer.cameraData.decalCompleteVec) {
        decalC->texDiff->bind(gl::TEXTURE0);
        decalC->texNorm->bind(gl::TEXTURE1);
        decalC->texSpec->bind(gl::TEXTURE2);
        decalC->ubo.bind(1u); sq::draw_volume_cube();
    }

    for (const DecalComponent* decalC : renderer.cameraData.decalPartialVec) {
        if (decalC->texDiff) decalC->texDiff->bind(gl::TEXTURE0);
        if (decalC->texNorm) decalC->texNorm->bind(gl::TEXTURE1);
        if (decalC->texSpec) decalC->texSpec->bind(gl::TEXTURE2);
        decalC->ubo.bind(1u); sq::draw_volume_cube();
    }

    sq::CULLFACE_BACK();
    sq::DEPTH_ON(); sq::STENCIL_REPLACE();
    gl::StencilFunc(gl::ALWAYS, 0b0001, 0b0000);
    renderer.pipeline.use_shader(FS_gbuf_base_model);
    renderer.pipeline.use_shader(VS_gbuf_base_simple);
    draw_msimples_base(false);
    renderer.pipeline.use_shader(VS_gbuf_base_skelly);
    draw_mskellys_base(false);

    FB_depth.attach(gl::DEPTH_ATTACHMENT, TEX_depHalf);
    FB_baseGbuf.blit(FB_depth, INFO_fullSize, INFO_halfSize, gl::DEPTH_BUFFER_BIT, gl::NEAREST);

    FB_depth.attach(gl::DEPTH_ATTACHMENT, TEX_depQter);
    FB_baseGbuf.blit(FB_depth, INFO_fullSize, INFO_qterSize, gl::DEPTH_BUFFER_BIT, gl::NEAREST);

    sq::FRONTFACE(false);
}


void Renderer::Gbuffers::draw_msimples_base(bool _decals) {
    for (const auto modelC : renderer.cameraData.modelSimpleVec) {
        if (modelC->PROP_decals != _decals) continue;
        sq::FRONTFACE(modelC->negScale); modelC->mesh->bind_vao(); modelC->ubo.bind(1u);
        for (uint i = 0u; i < modelC->mesh->mtrlCount; ++i)
            FS_gbuf_base_model.set_vec<Vec3I>("d_n_s", modelC->skin->mtrlVec[i].glDNS),
            modelC->skin->bind_textures(i), modelC->mesh->draw_material(i);
    }
}

void Renderer::Gbuffers::draw_mskellys_base(bool _decals) {
    for (const auto modelC : renderer.cameraData.modelSkellyVec) {
        if (modelC->PROP_decals != _decals) continue;
        sq::FRONTFACE(modelC->negScale); modelC->mesh->bind_vao(); modelC->ubo.bind(1u);
        for (uint i = 0u; i < modelC->mesh->mtrlCount; ++i)
            FS_gbuf_base_model.set_vec<Vec3I>("d_n_s", modelC->skin->mtrlVec[i].glDNS),
            modelC->skin->bind_textures(i), modelC->mesh->draw_material(i);
    }
}


void Renderer::Gbuffers::render_gbuffers_refl(const ReflectData& _data) {
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0100);
    renderer.pipeline.use_shader(FS_gbuf_refl_model);
    renderer.pipeline.use_shader(VS_gbuf_refl_simple);
    draw_msimples_refl(_data, true);
    renderer.pipeline.use_shader(VS_gbuf_refl_skelly);
    draw_mskellys_refl(_data, true);

    sq::CULLFACE_BACK();
    sq::DEPTH_OFF(); sq::STENCIL_KEEP();
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0101);
    renderer.pipeline.use_shader(VS_gbuf_refl_decal);
    renderer.pipeline.use_shader(FS_gbuf_refl_decal);

    for (const DecalComponent* decal : _data.decalDiffVec) {
        decal->texDiff->bind(gl::TEXTURE0);
        decal->ubo.bind(1u); sq::draw_volume_cube();
    }

    sq::CULLFACE_FRONT();
    sq::DEPTH_ON(); sq::STENCIL_REPLACE();
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0100);
    renderer.pipeline.use_shader(FS_gbuf_refl_model);
    renderer.pipeline.use_shader(VS_gbuf_refl_simple);
    draw_msimples_refl(_data, false);
    renderer.pipeline.use_shader(VS_gbuf_refl_skelly);
    draw_mskellys_refl(_data, false);

    sq::FRONTFACE(false);
}

void Renderer::Gbuffers::draw_msimples_refl(const ReflectData& _data, bool _decals) {
    for (const ModelComponent* modelC : _data.modelSimpleVec) {
        if (modelC->PROP_decals != _decals) continue;
        sq::FRONTFACE(modelC->negScale); modelC->mesh->bind_vao(); modelC->ubo.bind(1u);
        for (uint i = 0u; i < modelC->mesh->mtrlCount; ++i)
            FS_gbuf_refl_model.set_vec<Vec3I>("d_n_s", modelC->skin->mtrlVec[i].glDNS),
            modelC->skin->bind_textures(i), modelC->mesh->draw_material(i);
    }
}

void Renderer::Gbuffers::draw_mskellys_refl(const ReflectData& _data, bool _decals) {
    for (const ModelComponent* modelC : _data.modelSkellyVec) {
        if (modelC->PROP_decals != _decals) continue;
        sq::FRONTFACE(modelC->negScale); modelC->mesh->bind_vao(); modelC->ubo.bind(1u);
        for (uint i = 0u; i < modelC->mesh->mtrlCount; ++i)
            FS_gbuf_refl_model.set_vec<Vec3I>("d_n_s", modelC->skin->mtrlVec[i].glDNS),
            modelC->skin->bind_textures(i), modelC->mesh->draw_material(i);
    }
}


void Renderer::Gbuffers::update_options() {
    INFO_fullSize = options.WindowSize;
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
