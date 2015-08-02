#include <algorithm>
#include <glm/matrix.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Shaders.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/render/Armature.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>
#include <sqee/maths/Culling.hpp>
#include <sqee/maths/General.hpp>

#include "../wcoe/World.hpp"
#include "../wcoe/obj/ModelStatic.hpp"
#include "../wcoe/obj/ModelSkelly.hpp"
#include "../wcoe/obj/PointLight.hpp"
#include "../wcoe/obj/SpotLight.hpp"
#include "../wcoe/obj/Reflector.hpp"
#include "../wcoe/obj/Emitter.hpp"
#include "../wcoe/obj/Liquid.hpp"
#include "../wcoe/obj/Decal.hpp"
#include "Graph.hpp"

using namespace sqt::rndr;

void Graph::render_reflections() {
    for (const auto& rptr : reflectorList) {
        const wcoe::Reflector& rflct = *rptr.lock();
        if (glm::dot(camera->pos, rflct.normal) + rflct.offset < 0.f) continue;
        if (sq::bbox_in_frus(rflct.bbox, camera->frus)) continue;
        crntRflct = &rflct; rflct.ubo->bind(2);

        sq::DEPTH_ON(); sq::DEPTH_READ();
        sq::COLORMASK_ON(); sq::BLEND_OFF();
        sq::STENCIL_ON(); sq::STENCIL_REPLACE();
        sq::VIEWPORT(INFO.fullSize); FB.defrBase->use();
        gl::StencilMask(0b0010); sq::CLEAR_STENC();
        gl::StencilFunc(gl::EQUAL, 0b0011, 0b0001);
        pipeline->use_shader(*VS.gbuf_stencil_base);
        VS.gbuf_stencil_base->set_mat<fmat4>("matrix", rflct.matrix);
        pipeline->disable_stages(0, 0, 1); rflct.mesh->bind_vao();
        for (uint i = 0u; i < rflct.mesh->mtrlCount; i++) rflct.mesh->draw_ibo(i);

        FB.defrBase->bind(gl::READ_FRAMEBUFFER);
        FB.defrRefl->bind(gl::DRAW_FRAMEBUFFER);
        gl::BlitFramebuffer(0, 0, INFO.fullSize.x, INFO.fullSize.y,
                            0, 0, INFO.halfSize.x, INFO.halfSize.y,
                            gl::STENCIL_BUFFER_BIT, gl::NEAREST);
        sq::COLORMASK_OFF(); sq::DEPTH_WRITE();
        sq::VIEWPORT(INFO.halfSize); FB.defrRefl->use();
        gl::StencilMask(0b1100); sq::CLEAR_COLOR_DEPTH_STENC();

        TX.reflDiff->bind(gl::TEXTURE3);
        TX.reflSurf->bind(gl::TEXTURE4);
        TX.reflDpSt->bind(gl::TEXTURE7);

        sq::CLIP_ON();
        render_mstatics_refl(true);
        render_mskellys_refl(true);
        render_reflects_refl(true);
        render_decals_refl();
        render_mstatics_refl(false);
        render_mskellys_refl(false);
        render_reflects_refl(false);
        sq::CLIP_OFF();

        sq::CULLFACE_OFF();
        sq::DEPTH_OFF(); sq::STENCIL_ON();
        FB.hdrRefl->use(); sq::CLEAR_COLOR();

        render_skybox_refl();
        render_ambient_refl();
        render_skylight_refl();
        render_spotlights_refl();
        render_pointlights_refl();

        sq::BLEND_PREM(); sq::STENCIL_KEEP();
        gl::StencilFunc(gl::EQUAL, 0b0011, 0b0011);
        pipeline->use_shader(*VS.defr_reflectors);
        pipeline->use_shader(*FS.defr_reflectors);
        FB.hdrBase->use(); sq::VIEWPORT(INFO.fullSize);
        TX.hdrRefl->bind(gl::TEXTURE0);
        rflct.mesh->bind_vao();
        for (uint i = 0u; i < rflct.mesh->mtrlCount; i++)
            rflct.skin->bind_textures(i, 0, 0, 1),
            rflct.mesh->draw_ibo(i);
    }
}


void Graph::render_particles() {
    using PartData = wcoe::Emitter::PartData;

    static GLuint vao = 0u;
    static GLuint vbo = 0u;
    static GLuint ibo = 0u;
    static bool first = true;
    if (first) { first = false;
        gl::GenVertexArrays(1, &vao);
        gl::GenBuffers(1, &vbo);
        gl::GenBuffers(1, &ibo);

        gl::BindVertexArray(vao);
        gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
        gl::VertexAttribPointer(0, 4, gl::FLOAT, false, 32, (void*)(0));
        gl::VertexAttribPointer(1, 4, gl::FLOAT, false, 32, (void*)(16));
        gl::EnableVertexAttribArray(0); gl::EnableVertexAttribArray(1);
    }

    vector<PartData> partDataVec;
    for (const auto& eptr : emitterList) {
        const wcoe::Emitter& emitr = *eptr.lock();
        for (auto& p : emitr.partDataList) {
            sq::Sphere sphere = {fvec3(p.x, p.y, p.z), p.s};
            if (sq::sphr_in_frus(sphere, camera->frus)) continue;
            partDataVec.push_back(p);
        }
    }

    std::sort(partDataVec.begin(), partDataVec.end(),
              [this](const PartData& a, const PartData& b) {
        return glm::distance(camera->pos, fvec3(a.x,a.y,a.z))
             > glm::distance(camera->pos, fvec3(b.x,b.y,b.z)); });

    gl::BindVertexArray(vao);
    gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
    gl::BufferData(gl::ARRAY_BUFFER, 32*partDataVec.size(),
                   partDataVec.data(), gl::STATIC_DRAW);

    struct PartMeta {
        fvec2 pos; float rad; uint ind;
    }; vector<PartMeta> metaVec;
    for (uint i = 0u; i < partDataVec.size(); i++) {
        const auto p = partDataVec[i];
        fvec3 vPos = fvec3(camera->viewMat * fvec4(p.x,p.y,p.z, 1.f));
        fvec4 pPos = camera->projMat * fvec4(vPos, 1.f);
        fvec2 sPos = fvec2(pPos) / pPos.w;
        float d = glm::length(vPos);
        float l = glm::sqrt(d*d - p.s*p.s);
        float h = l * p.s / d;
        float sRad = 1.f / glm::tan(0.5f) * h / -vPos.z;
        sPos.x *= camera->size.x / camera->size.y;
        PartMeta pm = {sPos, sRad, i};
        metaVec.emplace_back(pm);
    } if (!metaVec.size()) return;

    list<vector<uint>> indVecList(1);
    for (const auto pm : metaVec) {
        for (uint ind : indVecList.back()) {
            const auto cmp = metaVec[ind];
            if (glm::distance(pm.pos, cmp.pos) < (pm.rad + cmp.rad))
                indVecList.emplace_back();
        } indVecList.back().emplace_back(pm.ind);
    }

    FB.hdrBase->bind(gl::READ_FRAMEBUFFER);
    FB.hdrPart->bind(gl::DRAW_FRAMEBUFFER);
    sq::DEPTH_ON(); sq::DEPTH_WRITE();
    sq::STENCIL_OFF(); sq::CLEAR_COLOR_DEPTH();
    gl::BlitFramebuffer(0, 0, INFO.fullSize.x, INFO.fullSize.y,
                        0, 0, INFO.halfSize.x, INFO.halfSize.y,
                        gl::DEPTH_BUFFER_BIT, gl::NEAREST);
    FB.hdrPart->use(); sq::VIEWPORT(INFO.halfSize);

    sq::CULLFACE_OFF();
    sq::BLEND_ON(); sq::DEPTH_READ();
    pipeline->use_shader(*VS.part_vertex_soft);
    pipeline->use_shader(*VS.part_geometry_soft);
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
    gl::ActiveTexture(gl::TEXTURE8);

    for (const auto& vec : indVecList) {
        gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, vec.size()*4, vec.data(), gl::STREAM_DRAW);

        sq::BLEND_PREM();
        world->ambient.ubo->bind(1);
        pipeline->use_shader(*FS.part_ambient_soft);
        gl::DrawElements(gl::POINTS, vec.size(), gl::UNSIGNED_INT, 0);
        sq::BLEND_ONEONE();

        pipeline->use_shader(*FS.part_skylight_soft);
        world->skylight.ubo->bind(1); world->skylight.texA->bind();
        gl::DrawElements(gl::POINTS, vec.size(), gl::UNSIGNED_INT, 0);

        for (const auto& lptr : spotLightList) {
            const wcoe::SpotLight& light = *lptr.lock();
            if (sq::frus_in_frus(light.frus, camera->frus)) continue; light.ubo->bind(1);
            if (light.PROP_shadow == false) pipeline->use_shader(*FS.part_spot_none_soft);
            else pipeline->use_shader(*FS.part_spot_shad_soft), light.tex->bind();
            gl::DrawElements(gl::POINTS, vec.size(), gl::UNSIGNED_INT, 0);
        }

        for (const auto& lptr : pointLightList) {
            const wcoe::PointLight& light = *lptr.lock();
            if (sq::sphr_in_frus(light.sphere, camera->frus)) continue; light.ubo->bind(1);
            if (light.PROP_shadow == false) pipeline->use_shader(*FS.part_point_none_soft);
            else pipeline->use_shader(*FS.part_point_shad_soft), light.tex->bind();
            gl::DrawElements(gl::POINTS, vec.size(), gl::UNSIGNED_INT, 0);
        }
    }

    sq::BLEND_PREM(); sq::DEPTH_OFF();
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.part_writefinal_soft);
    FB.hdrBase->use(); sq::VIEWPORT(INFO.fullSize);
    pipeline->disable_stages(0, 1, 0);
    TX.hdrPart->bind(gl::TEXTURE0);
    sq::draw_screen_quad();
}
