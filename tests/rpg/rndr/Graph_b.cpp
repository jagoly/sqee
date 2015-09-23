#include <glm/matrix.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
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
#include "../wcoe/objects/ModelSimple.hpp"
#include "../wcoe/objects/ModelSkelly.hpp"
#include "../wcoe/objects/PointLight.hpp"
#include "../wcoe/objects/SpotLight.hpp"
#include "../wcoe/objects/Reflector.hpp"
#include "../wcoe/objects/Emitter.hpp"
#include "../wcoe/objects/Liquid.hpp"
#include "../wcoe/objects/Decal.hpp"
#include "Graph.hpp"

using namespace sqt::rndr;

void Graph::render_shadows_sky_main() {
    if (!world->skylight.PROP_enabled) return;
    const wcoe::SkyLight& light = world->skylight;

    sq::DCLAMP_ON();
    sq::VIEWPORT(light.texDepthA->get_size());

    for (uint csm = 0u; csm < 4u; csm++) {
        light.fboArrA[csm]->use();
        sq::CLEAR_DEPTH();

        pipeline->use_shader(*VS.shad_simple);
        for (const auto& rptr : reflectorList) {
            const wcoe::Reflector& rflct = *rptr.lock();
            if (rflct.PROP_shadow == false) continue;
            if (sq::bbox_in_orth(rflct.bbox, light.orthArrA[csm])) continue;
            VS.shad_simple->set_mat<fmat4>("matrix", light.matArrA[csm] * rflct.matrix);
            sq::FRONTFACE(rflct.negScale); rflct.mesh->bind_vao();
            for (uint i = 0u; i < rflct.mesh->mtrlCount; i++) {
                if (rflct.skin->mtrlVec[i].punch == true) {
                    pipeline->use_shader(*FS.shad_punch);
                    rflct.skin->bind_textures(i, 1, 0, 0);
                } else pipeline->disable_stages(0, 0, 1);
                rflct.mesh->draw_ibo(i);
            }
        }

        pipeline->use_shader(*VS.shad_simple);
        for (const auto& mptr : modelSimpleList) {
            const wcoe::ModelSimple& model = *mptr.lock();
            if (model.PROP_shadow == false) continue;
            if (sq::bbox_in_orth(model.bbox, light.orthArrA[csm])) continue;
            VS.shad_simple->set_mat<fmat4>("matrix", light.matArrA[csm] * model.matrix);
            sq::FRONTFACE(model.negScale); model.mesh->bind_vao();
            for (uint i = 0u; i < model.mesh->mtrlCount; i++) {
                if (model.skin->mtrlVec[i].punch == true) {
                    pipeline->use_shader(*FS.shad_punch);
                    model.skin->bind_textures(i, 1, 0, 0);
                } else pipeline->disable_stages(0, 0, 1);
                model.mesh->draw_ibo(i);
            }
        }

        pipeline->use_shader(*VS.shad_skelly);
        for (const auto& mptr : modelSkellyList) {
            const wcoe::ModelSkelly& model = *mptr.lock();
            if (model.PROP_shadow == false) continue;
            if (sq::sphr_in_orth(model.sphere, light.orthArrA[csm])) continue;
            VS.shad_skelly->set_mat<fmat4>("matrix", light.matArrA[csm] * model.matrix);
            sq::FRONTFACE(model.negScale); model.ubo->bind(1); model.mesh->bind_vao();
            for (uint i = 0u; i < model.mesh->mtrlCount; i++) {
                if (model.skin->mtrlVec[i].punch == true) {
                    pipeline->use_shader(*FS.shad_punch);
                    model.skin->bind_textures(i, 1, 0, 0);
                } else pipeline->disable_stages(0, 0, 1);
                model.mesh->draw_ibo(i);
            }
        }
    } sq::FRONTFACE(false);
    sq::DCLAMP_OFF();
}


void Graph::render_shadows_sky_box() {
    if (!world->skylight.PROP_enabled) return;
    const wcoe::SkyLight& light = world->skylight;

    sq::DCLAMP_ON(); light.ubo->bind(1);
    sq::VIEWPORT(light.texDepthB->get_size());

    for (uint csm = 0u; csm < 2u; csm++) {
        light.fboArrB[csm]->use();
        sq::CLEAR_DEPTH();

        pipeline->use_shader(*VS.shad_simple);
        for (const auto& rptr : reflectorList) {
            const wcoe::Reflector& rflct = *rptr.lock();
            if (rflct.PROP_shadow == false) continue;
            if (sq::bbox_in_orth(rflct.bbox, light.orthArrB[csm])) continue;
            VS.shad_simple->set_mat<fmat4>("matrix", light.matArrB[csm] * rflct.matrix);
            sq::FRONTFACE(rflct.negScale); rflct.mesh->bind_vao();
            for (uint i = 0u; i < rflct.mesh->mtrlCount; i++) {
                if (rflct.skin->mtrlVec[i].punch == true) {
                    pipeline->use_shader(*FS.shad_punch);
                    rflct.skin->bind_textures(i, 1, 0, 0);
                } else pipeline->disable_stages(0, 0, 1);
                rflct.mesh->draw_ibo(i);
            }
        }

        pipeline->use_shader(*VS.shad_simple);
        for (const auto& mptr : modelSimpleList) {
            const wcoe::ModelSimple& model = *mptr.lock();
            if (model.PROP_shadow == false) continue;
            if (sq::bbox_in_orth(model.bbox, light.orthArrB[csm])) continue;
            VS.shad_simple->set_mat<fmat4>("matrix", light.matArrB[csm] * model.matrix);
            sq::FRONTFACE(model.negScale); model.mesh->bind_vao();
            for (uint i = 0u; i < model.mesh->mtrlCount; i++) {
                if (model.skin->mtrlVec[i].punch == true) {
                    pipeline->use_shader(*FS.shad_punch);
                    model.skin->bind_textures(i, 1, 0, 0);
                } else pipeline->disable_stages(0, 0, 1);
                model.mesh->draw_ibo(i);
            }
        }

        pipeline->use_shader(*VS.shad_skelly);
        for (const auto& mptr : modelSkellyList) {
            const wcoe::ModelSkelly& model = *mptr.lock();
            if (model.PROP_shadow == false) continue;
            if (sq::sphr_in_orth(model.sphere, light.orthArrB[csm])) continue;
            VS.shad_skelly->set_mat<fmat4>("matrix", light.matArrB[csm] * model.matrix);
            sq::FRONTFACE(model.negScale); model.ubo->bind(1); model.mesh->bind_vao();
            for (uint i = 0u; i < model.mesh->mtrlCount; i++) {
                if (model.skin->mtrlVec[i].punch == true) {
                    pipeline->use_shader(*FS.shad_punch);
                    model.skin->bind_textures(i, 1, 0, 0);
                } else pipeline->disable_stages(0, 0, 1);
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

        light.ubo->bind(1);
        sq::VIEWPORT(light.tex->get_size());
        light.fbo->use(); sq::CLEAR_DEPTH();
        pipeline->disable_stages(0, 0, 1);

        pipeline->use_shader(*VS.shad_simple);
        for (const auto& rptr : reflectorList) {
            const wcoe::Reflector& rflct = *rptr.lock();
            if (rflct.PROP_shadow == false) continue;
            if (sq::bbox_in_frus(rflct.bbox, light.frus)) continue;
            VS.shad_simple->set_mat<fmat4>("matrix", light.matrix * rflct.matrix);
            sq::FRONTFACE(rflct.negScale); rflct.mesh->bind_vao();
            for (uint i = 0u; i < rflct.mesh->mtrlCount; i++) {
                if (rflct.skin->mtrlVec[i].punch == true) {
                    pipeline->use_shader(*FS.shad_punch);
                    rflct.skin->bind_textures(i, 1, 0, 0);
                } else pipeline->disable_stages(0, 0, 1);
                rflct.mesh->draw_ibo(i);
            }
        }

        pipeline->use_shader(*VS.shad_simple);
        for (const auto& mptr : modelSimpleList) {
            const wcoe::ModelSimple& model = *mptr.lock();
            if (model.PROP_shadow == false) continue;
            if (sq::bbox_in_frus(model.bbox, light.frus)) continue;
            VS.shad_simple->set_mat<fmat4>("matrix", light.matrix * model.matrix);
            sq::FRONTFACE(model.negScale); model.mesh->bind_vao();
            for (uint i = 0u; i < model.mesh->mtrlCount; i++) {
                if (model.skin->mtrlVec[i].punch == true) {
                    pipeline->use_shader(*FS.shad_punch);
                    model.skin->bind_textures(i, 1, 0, 0);
                } else pipeline->disable_stages(0, 0, 1);
                model.mesh->draw_ibo(i);
            }
        }

        pipeline->use_shader(*VS.shad_skelly);
        for (const auto& mptr : modelSkellyList) {
            const wcoe::ModelSkelly& model = *mptr.lock();
            if (model.PROP_shadow == false) continue;
            if (sq::sphr_in_frus(model.sphere, light.frus)) continue;
            VS.shad_skelly->set_mat<fmat4>("matrix", light.matrix * model.matrix);
            sq::FRONTFACE(model.negScale); model.ubo->bind(1); model.mesh->bind_vao();
            for (uint i = 0u; i < model.mesh->mtrlCount; i++) {
                if (model.skin->mtrlVec[i].punch == true) {
                    pipeline->use_shader(*FS.shad_punch);
                    model.skin->bind_textures(i, 1, 0, 0);
                } else pipeline->disable_stages(0, 0, 1);
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
        sq::VIEWPORT(light.tex->get_size());
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

            pipeline->use_shader(*VS.shad_simple);
            for (const auto& rptr : reflectorList) {
                const wcoe::Reflector& rflct = *rptr.lock();
                if (rflct.PROP_shadow == false) continue;
                if (sq::bbox_in_frus(rflct.bbox, light.frusArr[face])) continue;
                VS.shad_simple->set_mat<fmat4>("matrix", light.matArr[face] * rflct.matrix);
                sq::FRONTFACE(rflct.negScale); rflct.mesh->bind_vao();
                for (uint i = 0u; i < rflct.mesh->mtrlCount; i++) {
                    if (rflct.skin->mtrlVec[i].punch == true) {
                        pipeline->use_shader(*FS.shad_punch);
                        rflct.skin->bind_textures(i, 1, 0, 0);
                    } else pipeline->disable_stages(0, 0, 1);
                    rflct.mesh->draw_ibo(i);
                }
            }

            pipeline->use_shader(*VS.shad_simple);
            for (const auto& mptr : modelSimpleList) {
                const wcoe::ModelSimple& model = *mptr.lock();
                if (model.PROP_shadow == false) continue;
                if (sq::bbox_in_frus(model.bbox, light.frusArr[face])) continue;
                VS.shad_simple->set_mat<fmat4>("matrix", light.matArr[face] * model.matrix);
                sq::FRONTFACE(model.negScale); model.mesh->bind_vao();
                for (uint i = 0u; i < model.mesh->mtrlCount; i++) {
                    if (model.skin->mtrlVec[i].punch == true) {
                        pipeline->use_shader(*FS.shad_punch);
                        model.skin->bind_textures(i, 1, 0, 0);
                    } else pipeline->disable_stages(0, 0, 1);
                    model.mesh->draw_ibo(i);
                }
            }

            pipeline->use_shader(*VS.shad_skelly);
            for (const auto& mptr : modelSkellyList) {
                const wcoe::ModelSkelly& model = *mptr.lock();
                if (model.PROP_shadow == false) continue;
                if (sq::sphr_in_frus(model.sphere, light.frusArr[face])) continue;
                VS.shad_skelly->set_mat<fmat4>("matrix", light.matArr[face] * model.matrix);
                sq::FRONTFACE(model.negScale); model.ubo->bind(1); model.mesh->bind_vao();
                for (uint i = 0u; i < model.mesh->mtrlCount; i++) {
                    if (model.skin->mtrlVec[i].punch == true) {
                        pipeline->use_shader(*FS.shad_punch);
                        model.skin->bind_textures(i, 1, 0, 0);
                    } else pipeline->disable_stages(0, 0, 1);
                    model.mesh->draw_ibo(i);
                }
            }
        }
    } sq::FRONTFACE(false);
}
