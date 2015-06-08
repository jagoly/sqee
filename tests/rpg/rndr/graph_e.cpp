#include <glm/matrix.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/app/settings.hpp>
#include <sqee/app/logging.hpp>
#include <sqee/gl/drawing.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/shaders.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/render/cameras.hpp>
#include <sqee/render/mesh.hpp>
#include <sqee/render/skin.hpp>
#include <sqee/render/skeleton.hpp>
#include <sqee/maths/culling.hpp>
#include <sqee/maths/general.hpp>

#include "../main/camera.hpp"
#include "../wcoe/world.hpp"
#include "../wcoe/obj/modelstatic.hpp"
#include "../wcoe/obj/modelskelly.hpp"
#include "../wcoe/obj/pointlight.hpp"
#include "../wcoe/obj/spotlight.hpp"
#include "../wcoe/obj/reflector.hpp"
#include "../wcoe/obj/emitter.hpp"
#include "../wcoe/obj/liquid.hpp"
#include "../wcoe/obj/decal.hpp"
#include "graph.hpp"

using namespace sqt;
using namespace rndr;

#define CLIP_ON gl::Enable(gl::CLIP_DISTANCE0)
#define CLIP_OFF gl::Disable(gl::CLIP_DISTANCE0)
#define COLORMASK_ON gl::ColorMask(0, 0, 0, 0)
#define COLORMASK_OFF gl::ColorMask(1, 1, 1, 1)
#define BLEND_PREM gl::Enable(gl::BLEND), gl::BlendFunc(gl::ONE, gl::ONE_MINUS_SRC_ALPHA)
#define BLEND_ALPHA gl::Enable(gl::BLEND), gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA)
#define BLEND_ONEONE gl::Enable(gl::BLEND), gl::BlendFunc(gl::ONE, gl::ONE)
#define BLEND_OFF gl::Disable(gl::BLEND)
#define CULLFACE_BACK gl::Enable(gl::CULL_FACE), gl::CullFace(gl::BACK)
#define CULLFACE_FRONT gl::Enable(gl::CULL_FACE), gl::CullFace(gl::FRONT)
#define CULLFACE_OFF gl::Disable(gl::CULL_FACE)
#define DEPTHTEST_RO gl::Enable(gl::DEPTH_TEST), gl::DepthMask(0)
#define DEPTHTEST_RW gl::Enable(gl::DEPTH_TEST), gl::DepthMask(1)
#define DEPTHTEST_OFF gl::Disable(gl::DEPTH_TEST)
#define STENCILTEST_ON gl::Enable(gl::STENCIL_TEST)
#define STENCILTEST_OFF gl::Disable(gl::STENCIL_TEST)
#define CLEAR_COLOR gl::Clear(gl::COLOR_BUFFER_BIT)
#define CLEAR_DEPTH gl::Clear(gl::DEPTH_BUFFER_BIT)
#define CLEAR_STENC gl::Clear(gl::STENCIL_BUFFER_BIT)
#define CLEAR_COLOR_DEPTH gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT)
#define CLEAR_COLOR_STENC gl::Clear(gl::COLOR_BUFFER_BIT | gl::STENCIL_BUFFER_BIT)
#define CLEAR_DEPTH_STENC gl::Clear(gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT)
#define CLEAR_COLOR_DEPTH_STENC gl::Clear(gl::COLOR_BUFFER_BIT|gl::DEPTH_BUFFER_BIT|gl::STENCIL_BUFFER_BIT)
#define VIEWPORT_FULL gl::Viewport(0, 0, INFO.fullSize.x, INFO.fullSize.y)
#define VIEWPORT_HALF gl::Viewport(0, 0, INFO.halfSize.x, INFO.halfSize.y)
#define VIEWPORT_QTER gl::Viewport(0, 0, INFO.qterSize.x, INFO.qterSize.y)


void Graph::render_reflections() {
    for (const auto& rptr : reflectorList) {
        const wcoe::Reflector& rflct = *rptr.lock();
        if (sq::bbox_in_frus(rflct.bbox, camera->frus)) continue;
        crntRflct = &rflct; rflct.ubo->bind(2);

        COLORMASK_ON; BLEND_OFF;
        DEPTHTEST_RO; STENCILTEST_ON;
        VIEWPORT_FULL; FB.defrBase->use();
        gl::StencilMask(0b0010); CLEAR_STENC;
        gl::StencilFunc(gl::EQUAL, 0b0011, 0b0001);
        gl::StencilOp(gl::KEEP, gl::KEEP, gl::REPLACE);
        pipeline->use_shader(*VS.gbuf_stencil_base);
        VS.gbuf_stencil_base->set_mat<fmat4>("matrix", rflct.matrix);
        pipeline->disable_stages(0, 0, 1); rflct.mesh->bind_vao();
        for (uint i = 0u; i < rflct.mesh->mCount; i++) rflct.mesh->draw_ibo(i);

        FB.defrBase->bind(gl::READ_FRAMEBUFFER);
        FB.defrRefl->bind(gl::DRAW_FRAMEBUFFER);
        gl::BlitFramebuffer(0, 0, INFO.fullSize.x, INFO.fullSize.y,
                            0, 0, INFO.halfSize.x, INFO.halfSize.y,
                            gl::STENCIL_BUFFER_BIT, gl::NEAREST);
        COLORMASK_OFF; DEPTHTEST_RW; FB.defrRefl->use();
        VIEWPORT_HALF; gl::StencilMask(0b1100);
        CLEAR_COLOR_DEPTH_STENC;

        TX.reflDiff->bind(gl::TEXTURE3);
        TX.reflSurf->bind(gl::TEXTURE4);
        TX.reflDpSt->bind(gl::TEXTURE7);

        render_mstatics_refl();
        render_reflects_refl();
        render_decals_refl();
        //render_mskellys_refl();

        CLIP_OFF; CULLFACE_OFF; DEPTHTEST_OFF;
        FB.hdrRefl->use(); CLEAR_COLOR;

        STENCILTEST_ON;
        BLEND_OFF; render_skybox_refl();
        BLEND_OFF; render_ambient_refl();
        BLEND_ONEONE; render_skylight_refl();
        BLEND_ONEONE; render_spotlights_refl();
        BLEND_ONEONE; render_pointlights_refl();

        STENCILTEST_ON; BLEND_ALPHA; CULLFACE_BACK;
        gl::StencilFunc(gl::EQUAL, 0b0011, 0b0011);
        gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
        pipeline->use_shader(*VS.defr_reflectors);
        pipeline->use_shader(*FS.defr_reflectors);
        FB.hdrBase->use(); VIEWPORT_FULL;
        TX.hdrRefl->bind(gl::TEXTURE0);
        rflct.mesh->bind_vao();
        for (uint i = 0u; i < rflct.mesh->mCount; i++)
            rflct.skin->bind_textures(i, 0, 0, 1),
            rflct.mesh->draw_ibo(i);
    }
}


//void Graph::render_particles() {
//    gl::Enable(gl::PROGRAM_POINT_SIZE);
//    gl::PointParameteri(gl::POINT_SPRITE_COORD_ORIGIN, gl::LOWER_LEFT);

//    for (const auto& eptr : emitterList) {
//        const wcoe::Emitter& emitr = *eptr.lock();
//        if (emitr.particleNum == 0u) continue;
//        crntEmitr = &emitr;

//        BLEND_OFF; DEPTHTEST_RW; STENCILTEST_ON;
//        FB.defrPart->use(); gl::StencilMask(0b1111);
//        VIEWPORT_HALF; CLEAR_COLOR_DEPTH_STENC;
//        TX.baseDpSt->bind(gl::TEXTURE8);
//        gl::BindVertexArray(emitr.vao);

//        CULLFACE_OFF; BLEND_PREM; DEPTHTEST_RW;
//        pipeline->use_shader(*VS.gbuf_particles);
//        pipeline->use_shader(*FS.gbuf_particles);
//        gl::StencilFunc(gl::ALWAYS, 0b0001, 0b0001);
//        gl::StencilOp(gl::KEEP, gl::KEEP, gl::REPLACE);
//        gl::DrawArrays(gl::POINTS, 0, emitr.particleNum);

//        DEPTHTEST_OFF;
//        TX.partMain->bind(gl::TEXTURE0);
//        TX.partDpSt->bind(gl::TEXTURE1);
//        gl::StencilFunc(gl::EQUAL, 0b0001, 0b0001);
//        gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
//        FB.hdrPart->use(); CLEAR_COLOR;
//        BLEND_OFF; render_ambient_part();
//        BLEND_ONEONE; render_skylight_part();

//        TX.partMain->bind(gl::TEXTURE0);
//        TX.hdrPart->bind(gl::TEXTURE1);

//        BLEND_ALPHA; CULLFACE_OFF;
//        DEPTHTEST_OFF; STENCILTEST_OFF;
//        pipeline->use_shader(*VS.gnrc_screen);
//        pipeline->use_shader(*FS.defr_particles);
//        FB.hdrBase->use(); VIEWPORT_FULL;
//        sq::draw_screen_quad();
//    }
//}


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

    std::list<vector<uint>> indVecList(1);
    for (const auto pm : metaVec) {
        for (uint ind : indVecList.back()) {
            const auto cmp = metaVec[ind];
            if (glm::distance(pm.pos, cmp.pos) < (pm.rad + cmp.rad))
                indVecList.emplace_back();
        } indVecList.back().emplace_back(pm.ind);
    }

    FB.hdrBase->bind(gl::READ_FRAMEBUFFER);
    FB.hdrPart->bind(gl::DRAW_FRAMEBUFFER);
    DEPTHTEST_RW; STENCILTEST_ON; CLEAR_COLOR_DEPTH_STENC;
    gl::BlitFramebuffer(0, 0, INFO.fullSize.x, INFO.fullSize.y,
                        0, 0, INFO.halfSize.x, INFO.halfSize.y,
                        gl::DEPTH_BUFFER_BIT, gl::NEAREST);
    FB.hdrPart->use(); VIEWPORT_HALF;

    CLIP_OFF; CULLFACE_OFF;
    DEPTHTEST_RO; STENCILTEST_OFF;
    pipeline->use_shader(*VS.part_vertex_soft);
    pipeline->use_shader(*VS.part_geometry_soft);
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
    gl::ActiveTexture(gl::TEXTURE8);

    for (const auto& vec : indVecList) {
        gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, vec.size()*4, vec.data(), gl::STREAM_DRAW);

        BLEND_PREM; world->ambient.ubo->bind(1);
        pipeline->use_shader(*FS.part_ambient_soft);
        gl::DrawElements(gl::POINTS, vec.size(), gl::UNSIGNED_INT, 0);
        BLEND_ONEONE;

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

    BLEND_PREM; DEPTHTEST_OFF;
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.part_writefinal_soft);
    pipeline->disable_stages(0, 1, 0);
    FB.hdrBase->use(); VIEWPORT_FULL;
    TX.hdrPart->bind(gl::TEXTURE0);
    sq::draw_screen_quad();
}
