#include <glm/matrix.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/app/settings.hpp>
#include <sqee/gl/drawing.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/shaders.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/render/camera.hpp>
#include <sqee/render/mesh.hpp>
#include <sqee/render/skin.hpp>
#include <sqee/render/skeleton.hpp>
#include <sqee/maths/culling.hpp>
#include <sqee/maths/general.hpp>

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

void Graph::render_mstatics_base() {
    gl::StencilMask(0b0100);
    sq::DEPTH_ON(); sq::DEPTH_WRITE();
    sq::CULLFACE_ON(); sq::CULLFACE_BACK();
    sq::STENCIL_ON(); sq::STENCIL_REPLACE();
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0001);
    pipeline->use_shader(*VS.gbuf_statics_base);
    pipeline->use_shader(*FS.gbuf_models_base);

    for (const auto& mptr : modelStaticList) {
        const wcoe::ModelStatic& model = *mptr.lock();
        if (model.PROP_render == false) continue;
        if (sq::bbox_in_frus(model.bbox, camera->frus)) continue;
        sq::FRONTFACE(model.negScale); model.ubo->bind(1); model.mesh->bind_vao();
        for (uint i = 0u; i < model.mesh->mCount; i++)
            FS.gbuf_models_base->set_vec<ivec3>("d_n_s", model.skin->mtrlVec[i].glDNS),
            model.skin->bind_textures(i), model.mesh->draw_ibo(i);
    } sq::FRONTFACE(false);
}


void Graph::render_mstatics_refl() {
    gl::StencilMask(0b0100);
    sq::DEPTH_ON(); sq::DEPTH_WRITE();
    sq::CULLFACE_ON(); sq::CULLFACE_FRONT();
    sq::STENCIL_ON(); sq::STENCIL_REPLACE();
    gl::StencilFunc(gl::EQUAL, 0b0111, 0b0011);
    pipeline->use_shader(*VS.gbuf_statics_refl);
    pipeline->use_shader(*FS.gbuf_models_refl);

    for (const auto& mptr : modelStaticList) {
        const wcoe::ModelStatic& model = *mptr.lock();
        if (model.PROP_render == false) continue;
        if (sq::bbox_in_frus(model.bbox, crntRflct->frus)) continue;
        sq::FRONTFACE(model.negScale); model.ubo->bind(1); model.mesh->bind_vao();
        for (uint i = 0u; i < model.mesh->mCount; i++)
            FS.gbuf_models_refl->set_vec<ivec3>("d_n_s", model.skin->mtrlVec[i].glDNS),
            model.skin->bind_textures(i), model.mesh->draw_ibo(i);
    } gl::FrontFace(gl::CCW);
}


void Graph::render_mskellys_base() {
    gl::StencilMask(0b0100);
    sq::DEPTH_ON(); sq::DEPTH_WRITE();
    sq::CULLFACE_ON(); sq::CULLFACE_BACK();
    sq::STENCIL_ON(); sq::STENCIL_REPLACE();
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0001);
    pipeline->use_shader(*VS.gbuf_skellys_base);
    pipeline->use_shader(*FS.gbuf_models_base);

    for (const auto& mptr : modelSkellyList) {
        const wcoe::ModelSkelly& model = *mptr.lock();
        if (model.PROP_render == false) continue;
        if (sq::bbox_in_frus(model.bbox, camera->frus)) continue;
        sq::FRONTFACE(model.negScale); model.ubo->bind(1); model.mesh->bind_vao();
        for (uint i = 0u; i < model.mesh->mCount; i++)
            FS.gbuf_models_base->set_vec<ivec3>("d_n_s", model.skin->mtrlVec[i].glDNS),
            model.skin->bind_textures(i), model.mesh->draw_ibo(i);
    } sq::FRONTFACE(false);
}


void Graph::render_mskellys_refl() {
    gl::StencilMask(0b0100);
    sq::DEPTH_ON(); sq::DEPTH_WRITE();
    sq::CULLFACE_ON(); sq::CULLFACE_FRONT();
    sq::STENCIL_ON(); sq::STENCIL_REPLACE();
    gl::StencilFunc(gl::EQUAL, 0b0111, 0b0011);
    pipeline->use_shader(*VS.gbuf_skellys_refl);
    pipeline->use_shader(*FS.gbuf_models_refl);

    for (const auto& mptr : modelSkellyList) {
        const wcoe::ModelSkelly& model = *mptr.lock();
        if (model.PROP_render == false) continue;
        if (sq::bbox_in_frus(model.bbox, crntRflct->frus)) continue;
        sq::FRONTFACE(model.negScale); model.ubo->bind(1); model.mesh->bind_vao();
        for (uint i = 0u; i < model.mesh->mCount; i++)
            FS.gbuf_models_refl->set_vec<ivec3>("d_n_s", model.skin->mtrlVec[i].glDNS),
            model.skin->bind_textures(i), model.mesh->draw_ibo(i);
    } sq::FRONTFACE(false);
}


void Graph::render_reflects_base() {
    gl::StencilMask(0b0100);
    sq::DEPTH_ON(); sq::DEPTH_WRITE();
    sq::CULLFACE_ON(); sq::CULLFACE_BACK();
    sq::STENCIL_ON(); sq::STENCIL_REPLACE();
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0001);
    pipeline->use_shader(*VS.gbuf_statics_base);
    pipeline->use_shader(*FS.gbuf_models_base);

    for (const auto& rptr : reflectorList) {
        const wcoe::Reflector& rflct = *rptr.lock();
        if (sq::bbox_in_frus(rflct.bbox, camera->frus)) continue;
        sq::FRONTFACE(rflct.negScale); rflct.ubo->bind(1); rflct.mesh->bind_vao();
        for (uint i = 0u; i < rflct.mesh->mCount; i++)
            FS.gbuf_models_base->set_vec<ivec3>("d_n_s", rflct.skin->mtrlVec[i].glDNS),
            rflct.skin->bind_textures(i), rflct.mesh->draw_ibo(i);
    } sq::FRONTFACE(false);
}


void Graph::render_reflects_refl() {
    gl::StencilMask(0b0100);
    sq::DEPTH_ON(); sq::DEPTH_WRITE();
    sq::CULLFACE_ON(); sq::CULLFACE_FRONT();
    sq::STENCIL_ON(); sq::STENCIL_REPLACE();
    gl::StencilFunc(gl::EQUAL, 0b0111, 0b0011);
    pipeline->use_shader(*VS.gbuf_statics_refl);
    pipeline->use_shader(*FS.gbuf_models_refl);

    for (const auto& rptr : reflectorList) {
        const wcoe::Reflector& rflct = *rptr.lock();
        if (sq::bbox_in_frus(rflct.bbox, crntRflct->frus)) continue;
        sq::FRONTFACE(rflct.negScale); rflct.ubo->bind(1); rflct.mesh->bind_vao();
        for (uint i = 0u; i < rflct.skin->mtrlVec.size(); i++)
            FS.gbuf_models_refl->set_vec<ivec3>("d_n_s", rflct.skin->mtrlVec[i].glDNS),
            rflct.skin->bind_textures(i), rflct.mesh->draw_ibo(i);
    } sq::FRONTFACE(false);
}


void Graph::render_decals_base() {
    sq::DEPTH_OFF();
    sq::STENCIL_ON(); sq::STENCIL_KEEP();
    sq::CULLFACE_ON(); sq::CULLFACE_FRONT();
    gl::StencilFunc(gl::EQUAL, 0b0101, 0b0101);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
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
    sq::DEPTH_OFF();
    sq::STENCIL_ON(); sq::STENCIL_KEEP();
    sq::CULLFACE_ON(); sq::CULLFACE_BACK();
    gl::StencilFunc(gl::EQUAL, 0b0111, 0b0111);
    pipeline->use_shader(*VS.gbuf_decals_refl);
    pipeline->use_shader(*FS.gbuf_decals_refl);

    for (const auto& dptr : decalList) {
        const wcoe::Decal& decal = *dptr.lock();
        if (sq::bbox_in_frus(decal.bbox, crntRflct->frus)) continue;
        if (decal.texDiff) decal.texDiff->bind(gl::TEXTURE0);
        else continue; decal.ubo->bind(1); sq::draw_volume_cube();
    }
}
