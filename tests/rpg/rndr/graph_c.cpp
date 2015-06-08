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


void Graph::render_mstatics_base() {
    STENCILTEST_ON; gl::StencilMask(0b0100);
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0001);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::REPLACE);
    CLIP_OFF; BLEND_OFF; CULLFACE_BACK; DEPTHTEST_RW;
    pipeline->use_shader(*VS.gbuf_statics_base);
    pipeline->use_shader(*FS.gbuf_models_base);

    for (const auto& mptr : modelStaticList) {
        const wcoe::ModelStatic& model = *mptr.lock();
        if (model.PROP_render == false) continue;
        if (sq::bbox_in_frus(model.bbox, camera->frus)) continue;
        gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
        model.ubo->bind(1); model.mesh->bind_vao();
        for (uint i = 0u; i < model.mesh->mCount; i++)
            FS.gbuf_models_base->set_vec<ivec3>("d_n_s", model.skin->mtrlVec[i].glDNS),
            model.skin->bind_textures(i), model.mesh->draw_ibo(i);
    } gl::FrontFace(gl::CCW);
}


void Graph::render_mstatics_refl() {
    STENCILTEST_ON; gl::StencilMask(0b0100);
    gl::StencilFunc(gl::EQUAL, 0b0111, 0b0011);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::REPLACE);
    CLIP_ON; BLEND_OFF; CULLFACE_FRONT; DEPTHTEST_RW;
    pipeline->use_shader(*VS.gbuf_statics_refl);
    pipeline->use_shader(*FS.gbuf_models_refl);

    for (const auto& mptr : modelStaticList) {
        const wcoe::ModelStatic& model = *mptr.lock();
        if (model.PROP_render == false) continue;
        if (sq::bbox_in_frus(model.bbox, crntRflct->frus)) continue;
        gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
        model.ubo->bind(1); model.mesh->bind_vao();
        for (uint i = 0u; i < model.mesh->mCount; i++)
            FS.gbuf_models_refl->set_vec<ivec3>("d_n_s", model.skin->mtrlVec[i].glDNS),
            model.skin->bind_textures(i), model.mesh->draw_ibo(i);
    } gl::FrontFace(gl::CCW);
}



//void Graph::render_mskellys(bool _reflect) {
//    STENCILTEST_ON; gl::StencilMask(0b1000);
//    gl::StencilFunc(gl::EQUAL, 0b1100, 0b0100);
//    gl::StencilOp(gl::KEEP, gl::KEEP, gl::REPLACE);
//    if (_reflect) CLIP_ON, CULLFACE_FRONT;
//    else CLIP_OFF, CULLFACE_BACK;
//    BLEND_OFF; DEPTHTEST_RW;

//    const sq::Frustum& frustum = _reflect ? reflFrus : camera->frus;
//    sq::Shader* skellysVS = _reflect ? VS.gbuf_skellys_refl : VS.gbuf_skellys_base;
//    sq::Shader* modelsFS = _reflect ? FS.gbuf_models_refl : FS.gbuf_models_base;
//    pipeline->use_shader(*skellysVS);
//    pipeline->use_shader(*modelsFS);

//    for (const auto& mptr : modelSkellyList) {
//        const wcoe::ModelSkelly& model = *mptr.lock();
//        if (model.DAT_render == false) continue;
//        if (sq::bbox_in_frus(model.bbox, frustum)) continue;
//        gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
//        skellyVS->set_mat<fmat4>("modelMat", model.matrix);
//        skellyVS->set_mat<fmat3>("normMat", sq::make_normMat(camera->viewMat * model.matrix));
//        skellyVS->set_vecptr<fvec4>("skelQuat", (float*)model.skel->quatVec.data());
//        skellyVS->set_vecptr<fvec3>("skelOffs", (float*)model.skel->offsVec.data());
//        model.mesh->bind_vao();
//        for (uint i = 0u; i < model.mesh->mCount; i++)
//            writeFS->set_vec<ivec3>("d_n_s", model.skin->mtrlVec[i].glDNS),
//            model.skin->bind_textures(i),
//            model.mesh->draw_ibo(i);
//    } gl::FrontFace(gl::CCW);
//}


void Graph::render_reflects_base() {
    STENCILTEST_ON; gl::StencilMask(0b0100);
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0001);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::REPLACE);
    CLIP_OFF; BLEND_OFF; CULLFACE_BACK; DEPTHTEST_RW;
    pipeline->use_shader(*VS.gbuf_statics_base);
    pipeline->use_shader(*FS.gbuf_models_base);

    for (const auto& rptr : reflectorList) {
        const wcoe::Reflector& rflct = *rptr.lock();
        if (sq::bbox_in_frus(rflct.bbox, camera->frus)) continue;
        gl::FrontFace(rflct.negScale ? gl::CW : gl::CCW);
        rflct.ubo->bind(1); rflct.mesh->bind_vao();
        for (uint i = 0u; i < rflct.mesh->mCount; i++)
            FS.gbuf_models_base->set_vec<ivec3>("d_n_s", rflct.skin->mtrlVec[i].glDNS),
            rflct.skin->bind_textures(i), rflct.mesh->draw_ibo(i);
    } gl::FrontFace(gl::CCW);
}


void Graph::render_reflects_refl() {
    STENCILTEST_ON; gl::StencilMask(0b0100);
    gl::StencilFunc(gl::EQUAL, 0b0111, 0b0011);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::REPLACE);
    CLIP_ON; BLEND_OFF; CULLFACE_FRONT; DEPTHTEST_RW;
    pipeline->use_shader(*VS.gbuf_statics_refl);
    pipeline->use_shader(*FS.gbuf_models_refl);

    for (const auto& rptr : reflectorList) {
        const wcoe::Reflector& rflct = *rptr.lock();
        if (sq::bbox_in_frus(rflct.bbox, crntRflct->frus)) continue;
        gl::FrontFace(rflct.negScale ? gl::CW : gl::CCW);
        rflct.ubo->bind(1); rflct.mesh->bind_vao();
        for (uint i = 0u; i < rflct.skin->mtrlVec.size(); i++)
            FS.gbuf_models_refl->set_vec<ivec3>("d_n_s", rflct.skin->mtrlVec[i].glDNS),
            rflct.skin->bind_textures(i), rflct.mesh->draw_ibo(i);
    } gl::FrontFace(gl::CCW);
}


void Graph::render_decals_base() {
    STENCILTEST_ON; gl::StencilMask(0b0000);
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0101);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
    CLIP_OFF; BLEND_OFF; CULLFACE_FRONT; DEPTHTEST_OFF;
    pipeline->use_shader(*VS.gbuf_decals_base);
    pipeline->use_shader(*FS.gbuf_decals_base);

    for (const auto& dptr : decalList) {
        const wcoe::Decal& decal = *dptr.lock();
        if (sq::bbox_in_frus(decal.bbox, camera->frus)) continue;
        if (decal.texDiff) decal.texDiff->bind(gl::TEXTURE0);
        if (decal.texNorm) decal.texNorm->bind(gl::TEXTURE1);
        if (decal.texSpec) decal.texSpec->bind(gl::TEXTURE2);
        decal.ubo->bind(1); sq::draw_volume_cube();
    }
}


void Graph::render_decals_refl() {
    STENCILTEST_ON; gl::StencilMask(0b0000);
    gl::StencilFunc(gl::EQUAL, 0b0111, 0b0111);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
    CLIP_ON; BLEND_OFF; CULLFACE_BACK; DEPTHTEST_OFF;
    pipeline->use_shader(*VS.gbuf_decals_refl);
    pipeline->use_shader(*FS.gbuf_decals_refl);

    for (const auto& dptr : decalList) {
        const wcoe::Decal& decal = *dptr.lock();
        if (sq::bbox_in_frus(decal.bbox, crntRflct->frus)) continue;
        if (decal.texDiff) decal.texDiff->bind(gl::TEXTURE0);
        else continue; decal.ubo->bind(1); sq::draw_volume_cube();
    }
}
