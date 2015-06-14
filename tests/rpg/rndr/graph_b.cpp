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

void Graph::render_shadows_sky_A() {
    if (!world->skylight.PROP_enabled) return;
    const wcoe::SkyLight& light = world->skylight;

    sq::DCLAMP_ON();
    light.ubo->bind(1);
    light.texA->viewport();

    for (uint csm = 0u; csm < 4u; csm++) {
        light.fboArrA[csm]->use();
        sq::CLEAR_DEPTH();

        pipeline->use_shader(*VS.shad_static);
        for (const auto& rptr : reflectorList) {
            const wcoe::Reflector& rflct = *rptr.lock();
            if (rflct.PROP_shadow == false) continue;
            VS.shad_static->set_mat<fmat4>("matrix", light.matArrA[csm] * rflct.matrix);
            sq::FRONTFACE(rflct.negScale); rflct.mesh->bind_vao();
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
            sq::FRONTFACE(model.negScale); model.mesh->bind_vao();
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
            if (model.PROP_shadow == false) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_skelly->set_mat<fmat4>("matrix", light.matArrA[csm] * model.matrix);
            sq::FRONTFACE(model.negScale); model.ubo->bind(1); model.mesh->bind_vao();
            for (uint i = 0u; i < model.mesh->mCount; i++) {
                if (model.skin->mtrlVec[i].punch == true)
                    pipeline->use_shader(*FS.shad_punch),
                    model.skin->bind_textures(i, 1, 0, 0);
                else pipeline->disable_stages(0, 0, 1);
                model.mesh->draw_ibo(i);
            }
        }
    } sq::FRONTFACE(false);
    sq::DCLAMP_OFF();
}


void Graph::render_shadows_sky_B() {
    if (!world->skylight.PROP_enabled) return;
    const wcoe::SkyLight& light = world->skylight;

    sq::DCLAMP_ON();
    light.ubo->bind(1);
    light.texB->viewport();

    for (uint csm = 0u; csm < 2u; csm++) {
        light.fboArrB[csm]->use();
        sq::CLEAR_DEPTH();

        pipeline->use_shader(*VS.shad_static);
        for (const auto& rptr : reflectorList) {
            const wcoe::Reflector& rflct = *rptr.lock();
            if (rflct.PROP_shadow == false) continue;
            VS.shad_static->set_mat<fmat4>("matrix", light.matArrB[csm] * rflct.matrix);
            sq::FRONTFACE(rflct.negScale); rflct.mesh->bind_vao();
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
            VS.shad_static->set_mat<fmat4>("matrix", light.matArrB[csm] * model.matrix);
            sq::FRONTFACE(model.negScale); model.mesh->bind_vao();
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
            if (model.PROP_shadow == false) continue;
            VS.shad_skelly->set_mat<fmat4>("matrix", light.matArrB[csm] * model.matrix);
            sq::FRONTFACE(model.negScale); model.ubo->bind(1); model.mesh->bind_vao();
            for (uint i = 0u; i < model.mesh->mCount; i++) {
                if (model.skin->mtrlVec[i].punch == true)
                    pipeline->use_shader(*FS.shad_punch),
                    model.skin->bind_textures(i, 1, 0, 0);
                else pipeline->disable_stages(0, 0, 1);
                model.mesh->draw_ibo(i);
            }
        }
    } sq::FRONTFACE(false);
    sq::DCLAMP_OFF();
}


void Graph::render_shadows_spot() {
    for (const auto& lptr : spotLightList) {
        const wcoe::SpotLight& light = *lptr.lock();
        if (light.PROP_shadow == false) continue;

        if (!sq::frus_in_frus(light.frus, camera->frus)) goto dontcull;
        for (const auto& rptr : reflectorList) {
            const wcoe::Reflector& rflct = *rptr.lock();
            if (glm::dot(camera->pos, rflct.normal) + rflct.offset < 0.f) continue;
            if (sq::bbox_in_frus(rflct.bbox, camera->frus)) continue;
            auto frus = sq::reflect_Frustum(camera->frus, rflct.normal, rflct.trans);
            if (!sq::frus_in_frus(light.frus, frus)) goto dontcull;
        } continue; dontcull:

        light.ubo->bind(1); light.tex->viewport();
        light.fbo->use(); sq::CLEAR_DEPTH();
        pipeline->disable_stages(0, 0, 1);

        pipeline->use_shader(*VS.shad_static);
        for (const auto& rptr : reflectorList) {
            const wcoe::Reflector& rflct = *rptr.lock();
            if (rflct.PROP_shadow == false) continue;
            if (sq::bbox_in_frus(rflct.bbox, light.frus)) continue;
            VS.shad_static->set_mat<fmat4>("matrix", light.matrix * rflct.matrix);
            sq::FRONTFACE(rflct.negScale); rflct.mesh->bind_vao();
            for (uint i = 0u; i < rflct.mesh->mCount; i++)
                rflct.mesh->draw_ibo(i);
        }

        pipeline->use_shader(*VS.shad_static);
        for (const auto& mptr : modelStaticList) {
            const wcoe::ModelStatic& model = *mptr.lock();
            if (model.PROP_shadow == false) continue;
            if (sq::bbox_in_frus(model.bbox, light.frus)) continue;
            VS.shad_static->set_mat<fmat4>("matrix", light.matrix * model.matrix);
            sq::FRONTFACE(model.negScale); model.mesh->bind_vao();
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
            if (model.PROP_shadow == false) continue;
            if (sq::bbox_in_frus(model.bbox, light.frus)) continue;
            VS.shad_skelly->set_mat<fmat4>("matrix", light.matrix * model.matrix);
            sq::FRONTFACE(model.negScale); model.ubo->bind(1); model.mesh->bind_vao();
            for (uint i = 0u; i < model.mesh->mCount; i++) {
                if (model.skin->mtrlVec[i].punch == true)
                    pipeline->use_shader(*FS.shad_punch),
                    model.skin->bind_textures(i, 1, 0, 0);
                else pipeline->disable_stages(0, 0, 1);
                model.mesh->draw_ibo(i);
            }
        }
    } sq::FRONTFACE(false);
}


void Graph::render_shadows_point() {
    for (const auto& lptr : pointLightList) {
        const wcoe::PointLight& light = *lptr.lock();
        if (light.PROP_shadow == false) continue;

        light.ubo->bind(1);
        light.tex->viewport();
        for (uint face = 0u; face < 6u; face++) {
            if (!sq::frus_in_frus(light.frusArr[face], camera->frus)) goto dontcull;
            for (const auto& rptr : reflectorList) {
                const wcoe::Reflector& rflct = *rptr.lock();
                if (glm::dot(camera->pos, rflct.normal) + rflct.offset < 0.f) continue;
                if (sq::bbox_in_frus(rflct.bbox, camera->frus)) continue;
                auto frus = sq::reflect_Frustum(camera->frus, rflct.normal, rflct.trans);
                if (!sq::frus_in_frus(light.frusArr[face], frus)) goto dontcull;
            } continue; dontcull:

            pipeline->disable_stages(0, 0, 1);
            light.fboArr[face]->use();
            sq::CLEAR_DEPTH();

            pipeline->use_shader(*VS.shad_static);
            for (const auto& rptr : reflectorList) {
                const wcoe::Reflector& rflct = *rptr.lock();
                if (rflct.PROP_shadow == false) continue;
                if (sq::bbox_in_frus(rflct.bbox, light.frusArr[face])) continue;
                VS.shad_static->set_mat<fmat4>("matrix", light.matArr[face] * rflct.matrix);
                sq::FRONTFACE(rflct.negScale); rflct.mesh->bind_vao();
                for (uint i = 0u; i < rflct.mesh->mCount; i++)
                    rflct.mesh->draw_ibo(i);
            }

            pipeline->use_shader(*VS.shad_static);
            for (const auto& mptr : modelStaticList) {
                const wcoe::ModelStatic& model = *mptr.lock();
                if (model.PROP_shadow == false) continue;
                if (sq::bbox_in_frus(model.bbox, light.frusArr[face])) continue;
                VS.shad_static->set_mat<fmat4>("matrix", light.matArr[face] * model.matrix);
                sq::FRONTFACE(model.negScale); model.mesh->bind_vao();
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
                if (model.PROP_shadow == false) continue;
                if (sq::bbox_in_frus(model.bbox, light.frusArr[face])) continue;
                VS.shad_skelly->set_mat<fmat4>("matrix", light.matArr[face] * model.matrix);
                sq::FRONTFACE(model.negScale); model.ubo->bind(1); model.mesh->bind_vao();
                for (uint i = 0u; i < model.mesh->mCount; i++) {
                    if (model.skin->mtrlVec[i].punch == true)
                        pipeline->use_shader(*FS.shad_punch),
                        model.skin->bind_textures(i, 1, 0, 0);
                    else pipeline->disable_stages(0, 0, 1);
                    model.mesh->draw_ibo(i);
                }
            }
        }
    } sq::FRONTFACE(false);
}
