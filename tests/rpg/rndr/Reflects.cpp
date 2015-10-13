#include <sqee/app/PreProcessor.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>

#include "../wcoe/World.hpp"
#include "../wcoe/SkyBox.hpp"
#include "../wcoe/Ambient.hpp"
#include "../wcoe/SkyLight.hpp"
#include "../wcoe/objects/SpotLight.hpp"
#include "../wcoe/objects/PointLight.hpp"
#include "../wcoe/objects/ModelSimple.hpp"
#include "../wcoe/objects/ModelSkelly.hpp"
#include "../wcoe/objects/Reflector.hpp"
#include "../wcoe/objects/Decal.hpp"
#include "Gbuffers.hpp"
#include "Pretties.hpp"
#include "Lighting.hpp"
#include "Reflects.hpp"

using namespace sqt::rndr;


Reflects::Reflects(const Renderer& _renderer) : renderer(_renderer) {
    renderer.preprocs(VS_defr_reflector, "deferred/reflector_vs");
    renderer.preprocs(FS_defr_reflector, "deferred/reflector_fs");
}


void Reflects::render_reflections() {
    for (const ReflectorData& data : renderer.reflectorDataVec) {
        data.rflct.ubo.bind(2u);

        sq::DEPTH_ON(); sq::DEPTH_READ();
        sq::BLEND_OFF(); sq::STENCIL_ON();
        sq::STENCIL_REPLACE();
        sq::VIEWPORT(renderer.INFO.fullSize);
        FB_stencil.bind(); gl::StencilMask(0b0100);
        gl::ClearStencil(0b0000); sq::CLEAR_STENC();
        gl::StencilFunc(gl::ALWAYS, 0b0100, 0b0000);
        renderer.pipeline.disable_stages(0, 0, 1);
        renderer.pipeline.use_shader(renderer.VS_stencil_base);
        renderer.VS_stencil_base.set_mat<fmat4>("matrix", data.rflct.matrix);
        data.rflct.mesh->bind_vao(); data.rflct.mesh->draw_complete();


        FB_stencil.blit(renderer.gbuffers->FB_reflGbuf, renderer.INFO.fullSize,
                        renderer.INFO.halfSize, gl::STENCIL_BUFFER_BIT, gl::NEAREST);


        sq::VIEWPORT(renderer.INFO.halfSize);
        sq::DEPTH_WRITE(); gl::StencilMask(0b0011);
        renderer.gbuffers->FB_reflGbuf.bind();
        sq::CLEAR_COLOR_DEPTH_STENC();

        sq::CLIP_ON();
        renderer.gbuffers->setup_render_state_refl();
        renderer.gbuffers->render_msimples_refl(data, true);
        renderer.gbuffers->render_mskellys_refl(data, true);
        renderer.gbuffers->render_reflects_refl(data, true);
        renderer.gbuffers->bind_textures_refl();
        renderer.gbuffers->render_decals_refl(data);
        renderer.gbuffers->render_msimples_refl(data, false);
        renderer.gbuffers->render_mskellys_refl(data, false);
        renderer.gbuffers->render_reflects_refl(data, false);
        sq::CLIP_OFF();


        sq::CULLFACE_OFF();
        sq::DEPTH_OFF(); sq::STENCIL_ON();
        renderer.lighting->FB_reflHdr.bind();
        sq::CLEAR_COLOR();

        renderer.lighting->render_skybox_refl();
        renderer.lighting->render_ambient_refl();
        renderer.lighting->render_skylight_refl();
        renderer.lighting->render_spotlights_refl(data);
        renderer.lighting->render_pointlights_refl(data);

        sq::BLEND_PREM(); sq::STENCIL_KEEP();
        gl::StencilFunc(gl::EQUAL, 0b0100, 0b0100);
        renderer.pipeline.use_shader(VS_defr_reflector);
        renderer.pipeline.use_shader(FS_defr_reflector);
        renderer.lighting->FB_baseHdr.bind();
        sq::VIEWPORT(renderer.INFO.fullSize);
        renderer.lighting->TEX_reflHdr.bind(gl::TEXTURE0);

        data.rflct.mesh->bind_vao();
        for (uint i = 0u; i < data.rflct.mesh->mtrlCount; ++i)
            data.rflct.skin->bind_textures(i, 0, 0, 1),
            data.rflct.mesh->draw_material(i);
    }
}


void Reflects::update_settings() {
    FB_stencil.attach(gl::DEPTH_STENCIL_ATTACHMENT, renderer.gbuffers->TEX_baseDpSt);
}


void Renderer::render_particles() {
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
}
