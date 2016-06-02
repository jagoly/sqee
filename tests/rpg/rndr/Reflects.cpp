#include <sqee/misc/StringCast.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>

#include "../components/Transform.hpp"
#include "../components/Model.hpp"
#include "../components/Reflect.hpp"

#include "Gbuffers.hpp"
#include "Lighting.hpp"
#include "Reflects.hpp"

using namespace sqt;

Renderer::Reflects::Reflects(const Renderer& _renderer)
    : renderer(_renderer), options(renderer.options) {

    FB_reflGbuf.draw_buffers({gl::COLOR_ATTACHMENT0, gl::COLOR_ATTACHMENT1});
    FB_reflHdr.draw_buffers({gl::COLOR_ATTACHMENT0});

    renderer.preprocs(VS_defr_reflector, "deferred/reflector_vs");
}


void Renderer::Reflects::render_reflections() {
    for (const ReflectData& data : renderer.reflectDataVec) {
        FB_stencil.bind(); gl::StencilMask(0b1111);
        sq::CLEAR_STENC(); gl::StencilMask(0b0100);
        sq::CULLFACE_ON(); sq::CULLFACE_BACK();
        sq::STENCIL_ON(); sq::DEPTH_READ();
        sq::DEPTH_ON(); sq::BLEND_OFF();
        sq::STENCIL_REPLACE();

        sq::FRONTFACE(data.model->negScale);
        gl::StencilFunc(gl::ALWAYS, 0b0100, 0b0000);

        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(renderer.VS_stencil_base);
        renderer.VS_stencil_base.set_mat<Mat4F>("matrix", data.model->matrix);
        sq::VIEWPORT(INFO_fullSize); data.model->mesh->bind_vao();
        data.model->mesh->draw_complete();

        FB_stencil.blit(FB_reflGbuf, INFO_fullSize, INFO_halfSize, gl::STENCIL_BUFFER_BIT, gl::NEAREST);

        data.reflect->ubo.bind(2u);
        TEX_reflDiff.bind(gl::TEXTURE3);
        TEX_reflSurf.bind(gl::TEXTURE4);
        TEX_reflDpSt.bind(gl::TEXTURE7);
        sq::VIEWPORT(INFO_halfSize);


        // GBuffers /////

        sq::CLIP_ON(); gl::StencilMask(0b0001);
        sq::DEPTH_WRITE(); sq::CULLFACE_FRONT();
        FB_reflGbuf.bind(); sq::CLEAR_COLOR_DEPTH();

        renderer.gbuffers->render_gbuffers_refl(data);


        // Lighting /////

        gl::StencilMask(0b0010);
        sq::CLIP_OFF(); sq::BLEND_ONEONE();
        sq::CULLFACE_OFF(); sq::DEPTH_READ();
        FB_reflHdr.bind(); sq::CLEAR_COLOR();

        renderer.lighting->render_lighting_refl(data);


        // Fill Space /////

        gl::StencilFunc(gl::EQUAL, 0b0000, 0b0100);
        sq::DEPTH_OFF(); sq::BLEND_OFF(); sq::STENCIL_KEEP();
        renderer.pipeline.use_shader(renderer.VS_fullscreen);
        renderer.pipeline.use_shader(renderer.FS_fill_space);

        FB_reflHdr.attach(gl::COLOR_ATTACHMENT0, TEX_reflHdrB);
        sq::CLEAR_COLOR();

        TEX_reflHdr.bind(gl::TEXTURE0); sq::draw_screen_quad();
        FB_reflHdr.attach(gl::COLOR_ATTACHMENT0, TEX_reflHdr);
        TEX_reflHdrB.bind(gl::TEXTURE0); sq::draw_screen_quad();


        // Composite /////

        sq::BLEND_PREM(); sq::STENCIL_KEEP();
        gl::StencilFunc(gl::EQUAL, 0b0100, 0b0100);
        sq::BLEND_ON(); sq::VIEWPORT(INFO_fullSize);
        renderer.pipeline.use_shader(VS_defr_reflector);
        renderer.pipeline.use_shader(FS_defr_reflector);
        renderer.lighting->FB_baseHdr.bind();
        TEX_reflHdr.bind(gl::TEXTURE0);

        data.model->ubo.bind(1u); data.model->mesh->bind_vao();
        for (uint i = 0u; i < data.model->mesh->mtrlCount; ++i)
            data.model->skin->bind_textures(i, 0, 0, 1),
            data.model->mesh->draw_material(i);
    }
}


void Renderer::Reflects::update_options() {
    INFO_fullSize = options.WindowSize;
    INFO_halfSize = INFO_fullSize / 2u;

    TEX_reflDiff.allocate_storage(INFO_halfSize, false);
    TEX_reflSurf.allocate_storage(INFO_halfSize, false);
    TEX_reflDpSt.allocate_storage(INFO_halfSize, false);

    TEX_reflHdr.allocate_storage(INFO_halfSize, false);
    TEX_reflHdrB.allocate_storage(INFO_halfSize, false);

    FB_reflGbuf.attach(gl::COLOR_ATTACHMENT0, TEX_reflDiff);
    FB_reflGbuf.attach(gl::COLOR_ATTACHMENT1, TEX_reflSurf);
    FB_reflGbuf.attach(gl::DEPTH_STENCIL_ATTACHMENT, TEX_reflDpSt);

    FB_reflHdr.attach(gl::COLOR_ATTACHMENT0, TEX_reflHdr);
    FB_reflHdr.attach(gl::DEPTH_STENCIL_ATTACHMENT, TEX_reflDpSt);

    FB_stencil.attach(gl::DEPTH_STENCIL_ATTACHMENT, renderer.gbuffers->TEX_baseDpSt);

    Vec2F hPixSize = Vec2F(1.f, 1.f) / Vec2F(INFO_halfSize);
    string defHPixSize = "#define PIXSIZE " + sq::glsl_string(hPixSize);
    renderer.preprocs(FS_defr_reflector, "deferred/reflector_fs", defHPixSize);
}


//void Renderer::Renderer::render_particles() {
//    using PartData = wcoe::Emitter::PartData;

//    vector<PartData> partDataVec;
//    for (const auto& eptr : emitterList) {
//        const wcoe::Emitter& emitr = *eptr.lock();
//        for (auto& p : emitr.partDataList) {
//            sq::Sphere sphere = {fvec3(p.x, p.y, p.z), p.s};
//            if (sq::sphr_in_frus(sphere, camera.frus)) continue;
//            partDataVec.push_back(p);
//        }
//    }

//    std::sort(partDataVec.begin(), partDataVec.end(),
//              [this](const PartData& a, const PartData& b) {
//        return glm::distance(camera.pos, fvec3(a.x,a.y,a.z))
//             > glm::distance(camera.pos, fvec3(b.x,b.y,b.z)); });

//    gl::BindVertexArray(partVAO);
//    gl::BindBuffer(gl::ARRAY_BUFFER, partVBO);
//    gl::BufferData(gl::ARRAY_BUFFER, 32*partDataVec.size(),
//                   partDataVec.data(), gl::STATIC_DRAW);

//    struct PartMeta {
//        fvec2 pos; float rad; uint ind;
//    }; vector<PartMeta> metaVec;
//    for (uint i = 0u; i < partDataVec.size(); i++) {
//        const auto p = partDataVec[i];
//        fvec3 vPos = fvec3(camera.viewMat * fvec4(p.x,p.y,p.z, 1.f));
//        fvec4 pPos = camera.projMat * fvec4(vPos, 1.f);
//        fvec2 sPos = fvec2(pPos) / pPos.w;
//        float d = glm::length(vPos);
//        float l = glm::sqrt(d*d - p.s*p.s);
//        float h = l * p.s / d;
//        float sRad = 1.f / glm::tan(0.5f) * h / -vPos.z;
//        sPos.x *= camera.size.x / camera.size.y;
//        PartMeta pm = {sPos, sRad, i};
//        metaVec.emplace_back(pm);
//    } if (!metaVec.size()) return;

//    list<vector<uint>> indVecList(1);
//    for (const auto pm : metaVec) {
//        for (uint ind : indVecList.back()) {
//            const auto cmp = metaVec[ind];
//            if (glm::distance(pm.pos, cmp.pos) < (pm.rad + cmp.rad))
//                indVecList.emplace_back();
//        } indVecList.back().emplace_back(pm.ind);
//    }

//    FB.hdrBase->bind(gl::READ_FRAMEBUFFER);
//    FB.hdrPart->bind(gl::DRAW_FRAMEBUFFER);
//    sq::DEPTH_ON(); sq::DEPTH_WRITE();
//    sq::STENCIL_OFF(); sq::CLEAR_COLOR_DEPTH();
//    gl::BlitFramebuffer(0, 0, INFO.fullSize.x, INFO.fullSize.y,
//                        0, 0, INFO.halfSize.x, INFO.halfSize.y,
//                        gl::DEPTH_BUFFER_BIT, gl::NEAREST);
//    FB.hdrPart->use(); sq::VIEWPORT(INFO.halfSize);

//    sq::CULLFACE_OFF();
//    sq::BLEND_ON(); sq::DEPTH_READ();
//    pipeline.use_shader(*VS.part_soft_vertex);
//    pipeline.use_shader(*GS.part_soft_geometry);
//    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, partIBO);
//    gl::ActiveTexture(gl::TEXTURE8);

//    for (const auto& vec : indVecList) {
//        gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, vec.size()*4, vec.data(), gl::STREAM_DRAW);

//        sq::BLEND_PREM();
//        world.ambient.ubo->bind(1);
//        pipeline.use_shader(*FS.part_soft_ambient);
//        gl::DrawElements(gl::POINTS, vec.size(), gl::UNSIGNED_INT, 0);
//        sq::BLEND_ONEONE();

//        pipeline.use_shader(*FS.part_soft_skylight);
//        world.skylight.ubo->bind(1); world.skylight.texDepthA->bind();
//        gl::DrawElements(gl::POINTS, vec.size(), gl::UNSIGNED_INT, 0);

//        for (const auto& lptr : spotLightList) {
//            const wcoe::SpotLight& light = *lptr.lock();
//            if (sq::frus_in_frus(light.frus, camera.frus)) continue; light.ubo->bind(1);
//            if (light.PROP_shadow == false) pipeline.use_shader(*FS.part_soft_spot_none);
//            else pipeline.use_shader(*FS.part_soft_spot_shad), light.tex->bind();
//            gl::DrawElements(gl::POINTS, vec.size(), gl::UNSIGNED_INT, 0);
//        }

//        for (const auto& lptr : pointLightList) {
//            const wcoe::PointLight& light = *lptr.lock();
//            if (sq::sphr_in_frus(light.sphere, camera.frus)) continue; light.ubo->bind(1);
//            if (light.PROP_shadow == false) pipeline.use_shader(*FS.part_soft_point_none);
//            else pipeline.use_shader(*FS.part_soft_point_shad), light.tex->bind();
//            gl::DrawElements(gl::POINTS, vec.size(), gl::UNSIGNED_INT, 0);
//        }
//    }

//    sq::BLEND_PREM(); sq::DEPTH_OFF();
//    pipeline.use_shader(*VS.gnrc_screen);
//    pipeline.use_shader(*FS.part_soft_write);
//    FB.hdrBase->use(); sq::VIEWPORT(INFO.fullSize);
//    pipeline.disable_stages(0, 1, 0);
//    TX.hdrPart->bind(gl::TEXTURE0);
//    sq::draw_screen_quad();
//}
