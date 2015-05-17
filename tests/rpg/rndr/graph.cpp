#include <glm/matrix.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/app/settings.hpp>
#include <sqee/app/logging.hpp>
#include <sqee/gl/misc.hpp>
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
#include "../wcoe/obj/spotlight.hpp"
#include "../wcoe/obj/pointlight.hpp"
#include "../wcoe/obj/reflector.hpp"
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


void Graph::update() {
    for (auto iter = modelStaticList.begin(); iter != modelStaticList.end(); iter++) {
        if (iter->second.expired()) { modelStaticList.erase(iter--); continue; }
        const wcoe::ModelStatic& obj = *iter->second.lock();
    }

    for (auto iter = modelSkellyList.begin(); iter != modelSkellyList.end(); iter++) {
        if (iter->second.expired()) { modelSkellyList.erase(iter--); continue; }
        const wcoe::ModelSkelly& obj = *iter->second.lock();
    }

    for (auto iter = spotLightList.begin(); iter != spotLightList.end(); iter++) {
        if (iter->second.expired()) { spotLightList.erase(iter--); continue; }
        const wcoe::SpotLight& obj = *iter->second.lock();
    }

    for (auto iter = pointLightList.begin(); iter != pointLightList.end(); iter++) {
        if (iter->second.expired()) { pointLightList.erase(iter--); continue; }
        const wcoe::PointLight& obj = *iter->second.lock();
    }

    for (auto iter = reflectorList.begin(); iter != reflectorList.end(); iter++) {
        if (iter->second.expired()) { reflectorList.erase(iter--); continue; }
        const wcoe::Reflector& obj = *iter->second.lock();
    }
}


void Graph::update_settings() {
    uint adjSize = 1024u * INFO.shadMult;
    world->skylight.texA->resize(uvec3(adjSize, adjSize, 4u));
    world->skylight.texB->resize(uvec3(adjSize, adjSize, 2u));
    if (INFO.shadFltr) world->skylight.texA->set_preset(sq::Texture2DArray::L_C()),
                       world->skylight.texB->set_preset(sq::Texture2DArray::L_C());
    else world->skylight.texA->set_preset(sq::Texture2DArray::N_C()),
         world->skylight.texB->set_preset(sq::Texture2DArray::N_C());

    for (const auto& lptr : spotLightList) {
        const wcoe::SpotLight& light = *lptr.second.lock();
        if (light.DAT_shadow == true) {
            adjSize = light.DAT_texsize * INFO.shadMult;
            light.tex->resize(uvec2(adjSize, adjSize));
            if (INFO.shadFltr) light.tex->set_preset(sq::Texture2D::L_C());
            else light.tex->set_preset(sq::Texture2D::N_C());
        }
    }
    for (const auto& lptr : pointLightList) {
        const wcoe::PointLight& light = *lptr.second.lock();
        if (light.DAT_shadow == true) {
            adjSize = light.DAT_texsize * INFO.shadMult;
            light.tex->resize(adjSize);
            if (INFO.shadFltr) light.tex->set_preset(sq::TextureCube::L_C());
            else light.tex->set_preset(sq::TextureCube::N_C());
        }
    }

    for (const auto& rptr : reflectorList) {
        const wcoe::Reflector& rflct = *rptr.second.lock();
        rflct.texDepth->resize(INFO.halfSize);
        rflct.texDiff->resize(INFO.halfSize);
        rflct.texSurf->resize(INFO.halfSize);
        rflct.texHdr->resize(INFO.halfSize);
    }
}


void Graph::update_from_world() {
    modelStaticList.clear();
    modelSkellyList.clear();
    spotLightList.clear();
    pointLightList.clear();
    reflectorList.clear();
    for (const auto& model : world->filtered<wcoe::ModelStatic>())
        modelStaticList.emplace_back(MetaModelStatic(), model);
    for (const auto& model : world->filtered<wcoe::ModelSkelly>())
        modelSkellyList.emplace_back(MetaModelSkelly(), model);
    for (const auto& light : world->filtered<wcoe::SpotLight>())
        spotLightList.emplace_back(MetaSpotLight(), light);
    for (const auto& light : world->filtered<wcoe::PointLight>())
        pointLightList.emplace_back(MetaPointLight(), light);
    for (const auto& rflct : world->filtered<wcoe::Reflector>())
        reflectorList.emplace_back(MetaReflector(), rflct);

    update(); update_settings();
}


void Graph::update_render_metadata() {

}


void Graph::render_models(bool _reflect) {
    STENCILTEST_ON; gl::StencilMask(0b1000);
    gl::StencilFunc(gl::EQUAL, 0b1100, 0b0100);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::REPLACE);
    if (_reflect) CLIP_ON, CULLFACE_FRONT;
    else CLIP_OFF, CULLFACE_BACK;
    BLEND_OFF; DEPTHTEST_RW;

    const sq::Frustum& frustum = _reflect ? reflFrus : camera->frus;
    sq::Shader* staticVS = _reflect ? VS.modl_static_refl : VS.modl_static;
    sq::Shader* skellyVS = _reflect ? VS.modl_skelly_refl : VS.modl_skelly;
    sq::Shader* writeFS = _reflect ? FS.modl_write_refl : FS.modl_write;
    pipeline->use_shader(*writeFS);

    pipeline->use_shader(*staticVS);
    for (const auto& mptr : modelStaticList) {
        const wcoe::ModelStatic& model = *mptr.second.lock();
        if (model.DAT_render == false) continue;
        if (sq::bbox_in_frus(model.bbox, frustum)) continue;
        gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
        staticVS->set_mat<mat4>("modelMat", model.matrix);
        staticVS->set_mat<mat3>("normMat", sq::make_normMat(camera->viewMat * model.matrix));
        model.mesh->bind_vao();
        for (uint i = 0u; i < model.mesh->mCount; i++)
            writeFS->set_sca<int>("diff_norm_spec", model.skin->mtrlVec[i].glDNS),
            model.skin->bind_textures(i),
            model.mesh->draw_ibo(i);
    }

    pipeline->use_shader(*skellyVS);
    for (const auto& mptr : modelSkellyList) {
        const wcoe::ModelSkelly& model = *mptr.second.lock();
        if (model.DAT_render == false) continue;
        if (sq::bbox_in_frus(model.bbox, frustum)) continue;
        gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
        skellyVS->set_mat<mat4>("modelMat", model.matrix);
        skellyVS->set_mat<mat3>("normMat", sq::make_normMat(camera->viewMat * model.matrix));
        skellyVS->set_vecptr<vec4>("skelQuat", (float*)model.skel->quatVec.data());
        skellyVS->set_vecptr<vec3>("skelOffs", (float*)model.skel->offsVec.data());
        model.mesh->bind_vao();
        for (uint i = 0u; i < model.mesh->mCount; i++)
            writeFS->set_sca<int>("diff_norm_spec", model.skin->mtrlVec[i].glDNS),
            model.skin->bind_textures(i),
            model.mesh->draw_ibo(i);
    } gl::FrontFace(gl::CCW);
}


void Graph::render_rflcts(bool _reflect) {
    STENCILTEST_ON; gl::StencilMask(0b1000);
    gl::StencilFunc(gl::EQUAL, 0b1100, 0b0100);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::REPLACE);
    if (_reflect) CLIP_ON, CULLFACE_FRONT;
    else CLIP_OFF, CULLFACE_BACK;
    BLEND_OFF; DEPTHTEST_RW;

    const sq::Frustum& frustum = _reflect ? reflFrus : camera->frus;
    sq::Shader* staticVS = _reflect ? VS.modl_static_refl : VS.modl_static;
    sq::Shader* writeFS = _reflect ? FS.modl_write_refl : FS.modl_write;

    pipeline->use_shader(*writeFS);
    pipeline->use_shader(*staticVS);

    for (const auto& rptr : reflectorList) {
        const wcoe::Reflector& rflct = *rptr.second.lock();
        if (sq::bbox_in_frus(rflct.bbox, frustum)) continue;
        gl::FrontFace(rflct.negScale ? gl::CW : gl::CCW);
        rflct.mesh->bind_vao();

        if (_reflect == false) {
            FB.defr->bind(gl::READ_FRAMEBUFFER);
            rflct.fboDefr->bind(gl::DRAW_FRAMEBUFFER);
            VIEWPORT_HALF; gl::StencilMask(0b1111);
            CLEAR_COLOR_DEPTH_STENC;

            gl::DepthMask(false); gl::StencilMask(0b0100);
            gl::StencilFunc(gl::ALWAYS, 0b0100, 0b0100);
            pipeline->use_shader(*VS.modl_stencil);
            VS.modl_stencil->set_mat("modelMat", rflct.matrix);
            pipeline->disable_stages(0, 0, 1);
            for (uint i = 0u; i < rflct.mesh->mCount; i++)
                rflct.mesh->draw_ibo(i);

            gl::DepthMask(true); gl::StencilMask(0b1000);
            gl::StencilFunc(gl::EQUAL, 0b1100, 0b0100);
            FB.defr->use(); VIEWPORT_FULL;
            pipeline->use_shader(*writeFS);
            pipeline->use_shader(*staticVS);
        }

        staticVS->set_mat<mat4>("modelMat", rflct.matrix);
        staticVS->set_mat<mat3>("normMat", sq::make_normMat(camera->viewMat * rflct.matrix));
        for (uint i = 0u; i < rflct.skin->mtrlVec.size(); i++)
            writeFS->set_sca<int>("diff_norm_spec", rflct.skin->mtrlVec[i].glDNS),
            rflct.skin->bind_textures(i),
            rflct.mesh->draw_ibo(i);
    } gl::FrontFace(gl::CCW);
}


void Graph::render_shadows_sky_A() {
    if (!world->skylight.enabled) return;
    const wcoe::SkyLight& light = world->skylight;

    CLIP_OFF; BLEND_OFF; STENCILTEST_OFF;
    CULLFACE_BACK; DEPTHTEST_RW;

    light.ubo->bind(1);
    light.texA->viewport();

    gl::Enable(gl::DEPTH_CLAMP);
    for (uint csm = 0u; csm < 4u; csm++) {
        light.fboArrA[csm]->use(); CLEAR_DEPTH;

        pipeline->use_shader(*VS.shad_static);
        for (const auto& rptr : reflectorList) {
            const wcoe::Reflector& rflct = *rptr.second.lock();
            if (rflct.DAT_shadow == false) continue;
            gl::FrontFace(rflct.negScale ? gl::CW : gl::CCW);
            VS.shad_static->set_mat<mat4>("matrix", light.matArrA[csm] * rflct.matrix);
            rflct.mesh->bind_vao();
            for (uint i = 0u; i < rflct.mesh->mCount; i++) {
                if (rflct.skin->mtrlVec[i].punch == true)
                    pipeline->use_shader(*FS.shad_punch),
                    rflct.skin->bind_textures(i, 1, 0, 0);
                else pipeline->disable_stages(0, 0, 1);
                rflct.mesh->draw_ibo(i);
            }
        }

        pipeline->use_shader(*VS.shad_static);
        for (const auto& mptr : modelStaticList) {
            const wcoe::ModelStatic& model = *mptr.second.lock();
            if (model.DAT_shadow == false) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_static->set_mat<mat4>("matrix", light.matArrA[csm] * model.matrix);
            model.mesh->bind_vao();
            for (uint i = 0u; i < model.mesh->mCount; i++) {
                if (model.skin->mtrlVec[i].punch == true)
                    pipeline->use_shader(*FS.shad_punch),
                    model.skin->bind_textures(i, 1, 0, 0);
                else pipeline->disable_stages(0, 0, 1);
                model.mesh->draw_ibo(i);
            }
        }

        pipeline->use_shader(*VS.shad_skelly);
        for (const auto& mptr : modelSkellyList) {
            const wcoe::ModelSkelly& model = *mptr.second.lock();
            if (model.DAT_shadow == false) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_skelly->set_mat<mat4>("matrix", light.matArrA[csm] * model.matrix);
            VS.shad_skelly->set_vecptr<vec4>("skelQuat", (float*)model.skel->quatVec.data());
            VS.shad_skelly->set_vecptr<vec3>("skelOffs", (float*)model.skel->offsVec.data());
            model.mesh->bind_vao();
            for (uint i = 0u; i < model.mesh->mCount; i++) {
                if (model.skin->mtrlVec[i].punch == true)
                    pipeline->use_shader(*FS.shad_punch),
                    model.skin->bind_textures(i, 1, 0, 0);
                else pipeline->disable_stages(0, 0, 1);
                model.mesh->draw_ibo(i);
            }
        }
    } gl::FrontFace(gl::CCW);
    gl::Disable(gl::DEPTH_CLAMP);
}


void Graph::render_shadows_sky_B() {
    if (!world->skylight.enabled) return;
    const wcoe::SkyLight& light = world->skylight;

    CLIP_OFF; BLEND_OFF; STENCILTEST_OFF;
    CULLFACE_BACK; DEPTHTEST_RW;

    light.ubo->bind(1);
    light.texB->viewport();

    gl::Enable(gl::DEPTH_CLAMP);
    for (uint csm = 0u; csm < 2u; csm++) {
        light.fboArrB[csm]->use(); CLEAR_DEPTH;

        pipeline->use_shader(*VS.shad_static);
        for (const auto& rptr : reflectorList) {
            const wcoe::Reflector& rflct = *rptr.second.lock();
            if (rflct.DAT_shadow == false) continue;
            gl::FrontFace(rflct.negScale ? gl::CW : gl::CCW);
            VS.shad_static->set_mat<mat4>("matrix", light.matArrB[csm] * rflct.matrix);
            rflct.mesh->bind_vao();
            for (uint i = 0u; i < rflct.mesh->mCount; i++) {
                if (rflct.skin->mtrlVec[i].punch == true)
                    pipeline->use_shader(*FS.shad_punch),
                    rflct.skin->bind_textures(i, 1, 0, 0);
                else pipeline->disable_stages(0, 0, 1);
                rflct.mesh->draw_ibo(i);
            }
        }

        pipeline->use_shader(*VS.shad_static);
        for (const auto& mptr : modelStaticList) {
            const wcoe::ModelStatic& model = *mptr.second.lock();
            if (model.DAT_shadow == false) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_static->set_mat<mat4>("matrix", light.matArrB[csm] * model.matrix);
            model.mesh->bind_vao();
            for (uint i = 0u; i < model.mesh->mCount; i++) {
                if (model.skin->mtrlVec[i].punch == true)
                    pipeline->use_shader(*FS.shad_punch),
                    model.skin->bind_textures(i, 1, 0, 0);
                else pipeline->disable_stages(0, 0, 1);
                model.mesh->draw_ibo(i);
            }
        }

        pipeline->use_shader(*VS.shad_skelly);
        for (const auto& mptr : modelSkellyList) {
            const wcoe::ModelSkelly& model = *mptr.second.lock();
            if (model.DAT_shadow == false) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_skelly->set_mat<mat4>("matrix", light.matArrB[csm] * model.matrix);
            VS.shad_skelly->set_vecptr<vec4>("skelQuat", (float*)model.skel->quatVec.data());
            VS.shad_skelly->set_vecptr<vec3>("skelOffs", (float*)model.skel->offsVec.data());
            model.mesh->bind_vao();
            for (uint i = 0u; i < model.mesh->mCount; i++) {
                if (model.skin->mtrlVec[i].punch == true)
                    pipeline->use_shader(*FS.shad_punch),
                    model.skin->bind_textures(i, 1, 0, 0);
                else pipeline->disable_stages(0, 0, 1);
                model.mesh->draw_ibo(i);
            }
        }
    } gl::FrontFace(gl::CCW);
    gl::Disable(gl::DEPTH_CLAMP);
}


void Graph::render_shadows_spot() {
    CLIP_OFF; BLEND_OFF; STENCILTEST_OFF;
    CULLFACE_BACK; DEPTHTEST_RW;

    for (const auto& lptr : spotLightList) {
        const wcoe::SpotLight& light = *lptr.second.lock();
        if (light.DAT_shadow == false) continue;

        //if (sq::frus_in_frus(light.frus, camera->frus)) continue;
//        if (!sq::frus_in_frus(light.frus, camera->frus)) goto dontcull;
//        for (const auto& rptr : reflectorList) {
//            const wcoe::Reflector& refl = *rptr.second.lock();
//            if (sq::bbox_in_frus(refl.bbox, camera->frus)) continue;
//            auto frus = sq::reflect_Frustum(camera->frus, refl.normal, refl.trans);
//            if (!sq::frus_in_frus(light.frus, frus)) goto dontcull;
//        } continue; dontcull:

        light.ubo->bind(1); light.tex->viewport();
        light.fbo->use(); CLEAR_DEPTH;

        pipeline->use_shader(*VS.shad_static);
        for (const auto& rptr : reflectorList) {
            const wcoe::Reflector& rflct = *rptr.second.lock();
            if (rflct.DAT_shadow == false) continue;
            if (sq::bbox_in_frus(rflct.bbox, light.frus)) continue;
            gl::FrontFace(rflct.negScale ? gl::CW : gl::CCW);
            VS.shad_static->set_mat<mat4>("matrix", light.matrix * rflct.matrix);
            rflct.mesh->bind_vao();
            for (uint i = 0u; i < rflct.mesh->mCount; i++) {
                if (rflct.skin->mtrlVec[i].punch == true)
                    pipeline->use_shader(*FS.shad_punch),
                    rflct.skin->bind_textures(i, 1, 0, 0);
                else pipeline->disable_stages(0, 0, 1);
                rflct.mesh->draw_ibo(i);
            }
        }

        pipeline->use_shader(*VS.shad_static);
        for (const auto& mptr : modelStaticList) {
            const wcoe::ModelStatic& model = *mptr.second.lock();
            if (model.DAT_shadow == false) continue;
            if (sq::bbox_in_frus(model.bbox, light.frus)) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_static->set_mat<mat4>("matrix", light.matrix * model.matrix);
            model.mesh->bind_vao();
            for (uint i = 0u; i < model.mesh->mCount; i++) {
                if (model.skin->mtrlVec[i].punch == true)
                    pipeline->use_shader(*FS.shad_punch),
                    model.skin->bind_textures(i, 1, 0, 0);
                else pipeline->disable_stages(0, 0, 1);
                model.mesh->draw_ibo(i);
            }
        }

        pipeline->use_shader(*VS.shad_skelly);
        for (const auto& mptr : modelSkellyList) {
            const wcoe::ModelSkelly& model = *mptr.second.lock();
            if (model.DAT_shadow == false) continue;
            if (sq::bbox_in_frus(model.bbox, light.frus)) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_skelly->set_mat<mat4>("matrix", light.matrix * model.matrix);
            VS.shad_skelly->set_vecptr<vec4>("skelQuat", (float*)model.skel->quatVec.data());
            VS.shad_skelly->set_vecptr<vec3>("skelOffs", (float*)model.skel->offsVec.data());
            model.mesh->bind_vao();
            for (uint i = 0u; i < model.mesh->mCount; i++) {
                if (model.skin->mtrlVec[i].punch == true)
                    pipeline->use_shader(*FS.shad_punch),
                    model.skin->bind_textures(i, 1, 0, 0);
                else pipeline->disable_stages(0, 0, 1);
                model.mesh->draw_ibo(i);
            }
        }
    } gl::FrontFace(gl::CCW);
}


void Graph::render_shadows_point() {
    CLIP_OFF; BLEND_OFF; STENCILTEST_OFF;
    CULLFACE_BACK; DEPTHTEST_RW;

    for (const auto& lptr : pointLightList) {
        const wcoe::PointLight& light = *lptr.second.lock();
        if (light.DAT_shadow == false) continue;

        if (sq::sphr_in_frus(light.sphere, camera->frus)) continue;
//        if (!sq::sphr_in_frus(light.sphere, camera->frus)) goto dontcull;
//        for (const auto& rptr : reflectorList) {
//            const wcoe::Reflector& refl = *rptr.second.lock();
//            if (sq::bbox_in_frus(refl.bbox, camera->frus)) continue;
//            auto frus = sq::reflect_Frustum(camera->frus, refl.normal, refl.trans);
//            if (!sq::sphr_in_frus(light.sphere, frus)) goto dontcull;
//        } continue; dontcull:

        light.ubo->bind(1);
        light.tex->viewport();
        for (uint face = 0u; face < 6u; face++) {
            if (sq::frus_in_frus(light.frusArr[face], camera->frus)) continue;
            light.fboArr[face]->use(); CLEAR_DEPTH;

            pipeline->use_shader(*VS.shad_static);
            for (const auto& rptr : reflectorList) {
                const wcoe::Reflector& rflct = *rptr.second.lock();
                if (rflct.DAT_shadow == false) continue;
                if (sq::bbox_in_frus(rflct.bbox, light.frusArr[face])) continue;
                gl::FrontFace(rflct.negScale ? gl::CW : gl::CCW);
                VS.shad_static->set_mat<mat4>("matrix", light.matArr[face] * rflct.matrix);
                rflct.mesh->bind_vao();
                for (uint i = 0u; i < rflct.mesh->mCount; i++) {
                    if (rflct.skin->mtrlVec[i].punch == true)
                        pipeline->use_shader(*FS.shad_punch),
                        rflct.skin->bind_textures(i, 1, 0, 0);
                    else pipeline->disable_stages(0, 0, 1);
                    rflct.mesh->draw_ibo(i);
                }
            }

            pipeline->use_shader(*VS.shad_static);
            for (const auto& mptr : modelStaticList) {
                const wcoe::ModelStatic& model = *mptr.second.lock();
                if (model.DAT_shadow == false) continue;
                if (sq::bbox_in_frus(model.bbox, light.frusArr[face])) continue;
                gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
                VS.shad_static->set_mat<mat4>("matrix", light.matArr[face] * model.matrix);
                model.mesh->bind_vao();
                for (uint i = 0u; i < model.mesh->mCount; i++) {
                    if (model.skin->mtrlVec[i].punch == true)
                        pipeline->use_shader(*FS.shad_punch),
                        model.skin->bind_textures(i, 1, 0, 0);
                    else pipeline->disable_stages(0, 0, 1);
                    model.mesh->draw_ibo(i);
                }
            }

            pipeline->use_shader(*VS.shad_skelly);
            for (const auto& mptr : modelSkellyList) {
                const wcoe::ModelSkelly& model = *mptr.second.lock();
                if (model.DAT_shadow == false) continue;
                if (sq::bbox_in_frus(model.bbox, light.frusArr[face])) continue;
                gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
                VS.shad_skelly->set_mat<mat4>("matrix", light.matArr[face] * model.matrix);
                VS.shad_skelly->set_vecptr<vec4>("skelQuat", (float*)model.skel->quatVec.data());
                VS.shad_skelly->set_vecptr<vec3>("skelOffs", (float*)model.skel->offsVec.data());
                model.mesh->bind_vao();
                for (uint i = 0u; i < model.mesh->mCount; i++) {
                    if (model.skin->mtrlVec[i].punch == true)
                        pipeline->use_shader(*FS.shad_punch),
                        model.skin->bind_textures(i, 1, 0, 0);
                    else pipeline->disable_stages(0, 0, 1);
                    model.mesh->draw_ibo(i);
                }
            }
        }
    } gl::FrontFace(gl::CCW);
}


void Graph::render_skybox(bool _reflect) {
    if (!world->skybox.enabled) return;

    if (!_reflect) pipeline->use_shader(*VS.shds_skybox);
    else pipeline->use_shader(*VS.shds_skybox_refl);

    gl::StencilFunc(gl::EQUAL, 0b0100, 0b1100);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
    pipeline->use_shader(*FS.shds_skybox);
    world->skybox.tex->bind(gl::TEXTURE5);
    sq::draw_screen_quad();
}


void Graph::render_ambient(bool _reflect) {
    if (!world->ambient.enabled) return;

    if (!_reflect) pipeline->use_shader(*FS.shds_ambient);
    else pipeline->use_shader(*FS.shds_ambient_refl);

    gl::StencilFunc(gl::EQUAL, 0b1100, 0b1100);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
    pipeline->use_shader(*VS.gnrc_screen);
    world->ambient.ubo->bind(1);
    sq::draw_screen_quad();
}


void Graph::render_skylight(bool _reflect) {
    if (!world->skylight.enabled) return;

    world->skylight.ubo->bind(1);
    pipeline->use_shader(*VS.gnrc_screen);

    if (_reflect == false) {
        pipeline->use_shader(*FS.shds_skylight);
        world->skylight.texA->bind(gl::TEXTURE5);
    } else {
        pipeline->use_shader(*FS.shds_skylight_refl);
        world->skylight.texB->bind(gl::TEXTURE5);
    }

    gl::StencilFunc(gl::EQUAL, 0b1100, 0b1100);
    gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
    sq::draw_screen_quad();
}


void Graph::render_spotlights(bool _reflect) {
    const sq::Frustum& frustum = _reflect ? reflFrus : camera->frus;

    pipeline->use_shader(*VS.gnrc_screen);
    for (const auto& lptr : spotLightList) {
        const wcoe::SpotLight& light = *lptr.second.lock();
        if (sq::frus_in_frus(light.frus, frustum)) continue;

        sq::Shader *stencilVS, *shadeFS;
        stencilVS = _reflect ? VS.modl_stencil_refl : VS.modl_stencil;
        if (_reflect) shadeFS = light.DAT_shadow ? FS.shds_spot_shad_refl : FS.shds_spot_none_refl;
        else if (!light.DAT_shadow && !light.DAT_specular) shadeFS = FS.shds_spot_none;
        else if ( light.DAT_shadow && !light.DAT_specular) shadeFS = FS.shds_spot_shad;
        else if (!light.DAT_shadow &&  light.DAT_specular) shadeFS = FS.shds_spot_spec;
        else if ( light.DAT_shadow &&  light.DAT_specular) shadeFS = FS.shds_spot_both;
        if (light.DAT_shadow) light.tex->bind(gl::TEXTURE5);
        light.ubo->bind(1);

        pipeline->use_shader(*stencilVS);
        stencilVS->set_mat("modelMat", light.modelMat);
        pipeline->disable_stages(0, 0, 1);
        gl::StencilMask(0b0001); CLEAR_STENC;
        gl::StencilFunc(gl::EQUAL, 0b1100, 0b1100);
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        DEPTHTEST_RO; sq::draw_volume_cone();
        DEPTHTEST_OFF; gl::StencilMask(0);

        pipeline->use_shader(*VS.gnrc_screen);
        pipeline->use_shader(*shadeFS);
        gl::StencilFunc(gl::EQUAL, 0b1101, 0b1101);
        gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
        sq::draw_screen_quad();
    }
}


void Graph::render_pointlights(bool _reflect) {
    const sq::Frustum& frustum = _reflect ? reflFrus : camera->frus;

    pipeline->use_shader(*VS.gnrc_screen);
    for (const auto& lptr : pointLightList) {
        const wcoe::PointLight& light = *lptr.second.lock();
        if (sq::sphr_in_frus(light.sphere, frustum)) continue;

        sq::Shader *stencilVS, *shadeFS;
        stencilVS = _reflect ? VS.modl_stencil_refl : VS.modl_stencil;
        if (_reflect) shadeFS = light.DAT_shadow ? FS.shds_point_shad_refl : FS.shds_point_none_refl;
        else if (!light.DAT_shadow && !light.DAT_specular) shadeFS = FS.shds_point_none;
        else if ( light.DAT_shadow && !light.DAT_specular) shadeFS = FS.shds_point_shad;
        else if (!light.DAT_shadow &&  light.DAT_specular) shadeFS = FS.shds_point_spec;
        else if ( light.DAT_shadow &&  light.DAT_specular) shadeFS = FS.shds_point_both;
        if (light.DAT_shadow) light.tex->bind(gl::TEXTURE5);
        light.ubo->bind(1);

        pipeline->use_shader(*stencilVS);
        stencilVS->set_mat("modelMat", light.modelMat);
        pipeline->disable_stages(0, 0, 1);
        gl::StencilMask(0b0001); CLEAR_STENC;
        gl::StencilFunc(gl::EQUAL, 0b1100, 0b1100);
        gl::StencilOp(gl::KEEP, gl::INVERT, gl::KEEP);
        DEPTHTEST_RO; sq::draw_volume_sphere();
        DEPTHTEST_OFF; gl::StencilMask(0);

        pipeline->use_shader(*VS.gnrc_screen);
        pipeline->use_shader(*shadeFS);
        gl::StencilFunc(gl::EQUAL, 0b1101, 0b1101);
        gl::StencilOp(gl::KEEP, gl::KEEP, gl::KEEP);
        sq::draw_screen_quad();
    }
}


void Graph::render_reflections() {
    for (const auto& rptr : reflectorList) {
        const wcoe::Reflector& rflct = *rptr.second.lock();
        if (sq::bbox_in_frus(rflct.bbox, camera->frus)) continue;

        rflct.ubo->bind(2);
        reflFrus = sq::reflect_Frustum(camera->frus, rflct.normal, rflct.trans);

        CLIP_OFF; BLEND_OFF; DEPTHTEST_RW;
        STENCILTEST_ON; gl::StencilMask(0b1111);
        rflct.fboDefr->use(); VIEWPORT_HALF;
        CLEAR_COLOR_DEPTH;

        render_models(true);
        render_rflcts(true);

        rflct.texDiff->bind(gl::TEXTURE0);
        rflct.texSurf->bind(gl::TEXTURE1);
        rflct.texDepth->bind(gl::TEXTURE4);

        CLIP_OFF; CULLFACE_OFF; DEPTHTEST_OFF;
        STENCILTEST_ON; gl::StencilMask(0b0000);
        rflct.fboHdr->use(); CLEAR_COLOR;

        BLEND_OFF; render_skybox(true);
        BLEND_OFF; render_ambient(true);
        BLEND_ONEONE; render_skylight(true);
        BLEND_ONEONE; render_spotlights(true);
        BLEND_ONEONE; render_pointlights(true);

        STENCILTEST_OFF; BLEND_ALPHA; CULLFACE_BACK;
        DEPTHTEST_RO; gl::DepthFunc(gl::LEQUAL);
        rflct.texHdr->bind(gl::TEXTURE0);
        FB.hdr->use(); VIEWPORT_FULL;
        pipeline->use_shader(*VS.shds_reflector);
        pipeline->use_shader(*FS.shds_reflector);
        rflct.mesh->bind_vao();
        for (uint i = 0u; i < rflct.mesh->mCount; i++)
            rflct.skin->bind_textures(i, 0, 0, 1),
            rflct.mesh->draw_ibo(i);
    }
}
