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


void Graph::render_shadows_sky_A() {
    if (!world->skylight.PROP_enabled) return;
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
            const wcoe::Reflector& rflct = *rptr.lock();
            if (rflct.PROP_shadow == false) continue;
            gl::FrontFace(rflct.negScale ? gl::CW : gl::CCW);
            VS.shad_static->set_mat<fmat4>("matrix", light.matArrA[csm] * rflct.matrix);
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
            const wcoe::ModelStatic& model = *mptr.lock();
            if (model.PROP_shadow == false) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_static->set_mat<fmat4>("matrix", light.matArrA[csm] * model.matrix);
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
            const wcoe::ModelSkelly& model = *mptr.lock();
            if (model.DAT_shadow == false) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_skelly->set_mat<fmat4>("matrix", light.matArrA[csm] * model.matrix);
            VS.shad_skelly->set_vecptr<fvec4>("skelQuat", (float*)model.skel->quatVec.data());
            VS.shad_skelly->set_vecptr<fvec3>("skelOffs", (float*)model.skel->offsVec.data());
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
    if (!world->skylight.PROP_enabled) return;
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
            const wcoe::Reflector& rflct = *rptr.lock();
            if (rflct.PROP_shadow == false) continue;
            gl::FrontFace(rflct.negScale ? gl::CW : gl::CCW);
            VS.shad_static->set_mat<fmat4>("matrix", light.matArrB[csm] * rflct.matrix);
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
            const wcoe::ModelStatic& model = *mptr.lock();
            if (model.PROP_shadow == false) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_static->set_mat<fmat4>("matrix", light.matArrB[csm] * model.matrix);
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
            const wcoe::ModelSkelly& model = *mptr.lock();
            if (model.DAT_shadow == false) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_skelly->set_mat<fmat4>("matrix", light.matArrB[csm] * model.matrix);
            VS.shad_skelly->set_vecptr<fvec4>("skelQuat", (float*)model.skel->quatVec.data());
            VS.shad_skelly->set_vecptr<fvec3>("skelOffs", (float*)model.skel->offsVec.data());
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
        const wcoe::SpotLight& light = *lptr.lock();
        if (light.PROP_shadow == false) continue;

        if (!sq::frus_in_frus(light.frus, camera->frus)) goto dontcull;
        for (const auto& rptr : reflectorList) {
            const wcoe::Reflector& refl = *rptr.lock();
            if (sq::bbox_in_frus(refl.bbox, camera->frus)) continue;
            auto frus = sq::reflect_Frustum(camera->frus, refl.normal, refl.trans);
            if (!sq::frus_in_frus(light.frus, frus)) goto dontcull;
        } continue; dontcull:

        light.ubo->bind(1); light.tex->viewport();
        pipeline->disable_stages(0, 0, 1);
        light.fbo->use(); CLEAR_DEPTH;

        pipeline->use_shader(*VS.shad_static);
        for (const auto& rptr : reflectorList) {
            const wcoe::Reflector& rflct = *rptr.lock();
            if (rflct.PROP_shadow == false) continue;
            if (sq::bbox_in_frus(rflct.bbox, light.frus)) continue;
            gl::FrontFace(rflct.negScale ? gl::CW : gl::CCW);
            VS.shad_static->set_mat<fmat4>("matrix", light.matrix * rflct.matrix);
            rflct.mesh->bind_vao();
            for (uint i = 0u; i < rflct.mesh->mCount; i++)
                rflct.mesh->draw_ibo(i);
        }

        pipeline->use_shader(*VS.shad_static);
        for (const auto& mptr : modelStaticList) {
            const wcoe::ModelStatic& model = *mptr.lock();
            if (model.PROP_shadow == false) continue;
            if (sq::bbox_in_frus(model.bbox, light.frus)) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_static->set_mat<fmat4>("matrix", light.matrix * model.matrix);
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
            const wcoe::ModelSkelly& model = *mptr.lock();
            if (model.DAT_shadow == false) continue;
            if (sq::bbox_in_frus(model.bbox, light.frus)) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_skelly->set_mat<fmat4>("matrix", light.matrix * model.matrix);
            VS.shad_skelly->set_vecptr<fvec4>("skelQuat", (float*)model.skel->quatVec.data());
            VS.shad_skelly->set_vecptr<fvec3>("skelOffs", (float*)model.skel->offsVec.data());
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
        const wcoe::PointLight& light = *lptr.lock();
        if (light.PROP_shadow == false) continue;

        light.ubo->bind(1);
        light.tex->viewport();
        for (uint face = 0u; face < 6u; face++) {
            if (!sq::frus_in_frus(light.frusArr[face], camera->frus)) goto dontcull;
            for (const auto& rptr : reflectorList) {
                const wcoe::Reflector& rflct = *rptr.lock();
                if (sq::bbox_in_frus(rflct.bbox, camera->frus)) continue;
                auto frus = sq::reflect_Frustum(camera->frus, rflct.normal, rflct.trans);
                if (!sq::frus_in_frus(light.frusArr[face], frus)) goto dontcull;
            } continue; dontcull:

            pipeline->disable_stages(0, 0, 1);
            light.fboArr[face]->use(); CLEAR_DEPTH;

            pipeline->use_shader(*VS.shad_static);
            for (const auto& rptr : reflectorList) {
                const wcoe::Reflector& rflct = *rptr.lock();
                if (rflct.PROP_shadow == false) continue;
                if (sq::bbox_in_frus(rflct.bbox, light.frusArr[face])) continue;
                gl::FrontFace(rflct.negScale ? gl::CW : gl::CCW);
                VS.shad_static->set_mat<fmat4>("matrix", light.matArr[face] * rflct.matrix);
                rflct.mesh->bind_vao();
                for (uint i = 0u; i < rflct.mesh->mCount; i++)
                    rflct.mesh->draw_ibo(i);
            }

            pipeline->use_shader(*VS.shad_static);
            for (const auto& mptr : modelStaticList) {
                const wcoe::ModelStatic& model = *mptr.lock();
                if (model.PROP_shadow == false) continue;
                if (sq::bbox_in_frus(model.bbox, light.frusArr[face])) continue;
                gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
                VS.shad_static->set_mat<fmat4>("matrix", light.matArr[face] * model.matrix);
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
                const wcoe::ModelSkelly& model = *mptr.lock();
                if (model.DAT_shadow == false) continue;
                if (sq::bbox_in_frus(model.bbox, light.frusArr[face])) continue;
                gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
                VS.shad_skelly->set_mat<fmat4>("matrix", light.matArr[face] * model.matrix);
                VS.shad_skelly->set_vecptr<fvec4>("skelQuat", (float*)model.skel->quatVec.data());
                VS.shad_skelly->set_vecptr<fvec3>("skelOffs", (float*)model.skel->offsVec.data());
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
