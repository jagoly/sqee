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
#include <sqee/render/lightsky.hpp>
#include <sqee/render/lightspot.hpp>
#include <sqee/render/lightpoint.hpp>

#include "../main/camera.hpp"
#include "../wcoe/obj/modelstatic.hpp"
#include "../wcoe/obj/modelskelly.hpp"
#include "../wcoe/obj/lightsky.hpp"
#include "../wcoe/obj/lightspot.hpp"
#include "../wcoe/obj/lightpoint.hpp"
#include "graph.hpp"

using namespace sqt::rndr;

void Graph::update() {
    for (auto iter = modelStaticList.begin(); iter != modelStaticList.end(); iter++) {
        if (iter->expired()) { modelStaticList.erase(iter--); continue; }
        const wcoe::ModelStatic& obj = *iter->lock();
    }

    for (auto iter = modelSkellyList.begin(); iter != modelSkellyList.end(); iter++) {
        if (iter->expired()) { modelSkellyList.erase(iter--); continue; }
        const wcoe::ModelSkelly& obj = *iter->lock();
    }

    for (auto iter = lightSkyList.begin(); iter != lightSkyList.end(); iter++) {
        if (iter->expired()) { lightSkyList.erase(iter--); continue; }
        const wcoe::LightSky& obj = *iter->lock();
    }

    for (auto iter = lightSpotList.begin(); iter != lightSpotList.end(); iter++) {
        if (iter->expired()) { lightSpotList.erase(iter--); continue; }
        const wcoe::LightSpot& obj = *iter->lock();
    }

    for (auto iter = lightPointList.begin(); iter != lightPointList.end(); iter++) {
        if (iter->expired()) { lightPointList.erase(iter--); continue; }
        const wcoe::LightPoint& obj = *iter->lock();
    }
}


void Graph::update_settings() {
    for (const auto& lptr : lightSkyList) {
        const wcoe::LightSky& light = *lptr.lock();
        if (light.DATA.shadow == true) {
            light.sSky->resize_texture(settings->crnt<int>("shadQuality"));
            light.sSky->filter_texture(settings->crnt<int>("shadFilter"));
        }
    }
    for (const auto& lptr : lightSpotList) {
        const wcoe::LightSpot& light = *lptr.lock();
        if (light.DATA.shadow == true) {
            light.sSpot->resize_texture(settings->crnt<int>("shadQuality"));
            light.sSpot->filter_texture(settings->crnt<int>("shadFilter"));
        }
    }
    for (const auto& lptr : lightPointList) {
        const wcoe::LightPoint& light = *lptr.lock();
        if (light.DATA.shadow == true) {
            light.sPoint->resize_texture(settings->crnt<int>("shadQuality"));
            light.sPoint->filter_texture(settings->crnt<int>("shadFilter"));
        }
    }
}


void Graph::render_models() {
    pipeline->use_shader(*FS.modl_write);

    pipeline->use_shader(*VS.modl_static);
    for (const auto& mptr : modelStaticList) {
        const wcoe::ModelStatic& model = *mptr.lock();
        if (model.DATA.render == false) continue;
        if (sq::bbox_in_frus(model.bbox, camera->frus)) continue;
        gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
        VS.modl_static->set_mat<mat4>("modelMat", model.matrix);
        VS.modl_static->set_mat<mat3>("normMat", sq::make_normMat(camera->viewMat * model.matrix));
        model.mesh->bind_vao();
        for (uint i = 0u; i < model.skin->mtrlVec.size(); i++)
            FS.modl_write->set_sca<int>("diff_norm_spec", model.skin->mtrlVec[i].glDNS),
            model.skin->bind_textures(i),
            model.mesh->draw_ibo(i);
    }

    pipeline->use_shader(*VS.modl_skelly);
    for (const auto& mptr : modelSkellyList) {
        const wcoe::ModelSkelly& model = *mptr.lock();
        if (model.DATA.render == false) continue;
        if (sq::bbox_in_frus(model.bbox, camera->frus)) continue;
        gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
        VS.modl_skelly->set_mat<mat4>("modelMat", model.matrix);
        VS.modl_skelly->set_mat<mat3>("normMat", sq::make_normMat(camera->viewMat * model.matrix));
        VS.modl_skelly->set_vecptr<vec4>("skelQuat", (float*)model.skel->quatVec.data());
        VS.modl_skelly->set_vecptr<vec3>("skelOffs", (float*)model.skel->offsVec.data());
        model.mesh->bind_vao();
        for (uint i = 0u; i < model.skin->mtrlVec.size(); i++)
            FS.modl_write->set_sca<int>("diff_norm_spec", model.skin->mtrlVec[i].glDNS),
            model.skin->bind_textures(i),
            model.mesh->draw_ibo(i);
    }
}


void Graph::render_shadows_sky() {
    for (const auto& lptr : lightSkyList) {
        const wcoe::LightSky& light = *lptr.lock();
        if (light.DATA.shadow == false) continue;

        light.sSky->ubo->bind(1);
        gl::Viewport(0, 0, light.sSky->tex->size.x, light.sSky->tex->size.y);
        gl::Enable(gl::DEPTH_CLAMP);
        for (int csm = 0; csm < 4; csm++) {
            light.sSky->fboArr[csm]->use();
            gl::Clear(gl::DEPTH_BUFFER_BIT);

            pipeline->use_shader(*VS.shad_static);
            for (const auto& mptr : modelStaticList) {
                const wcoe::ModelStatic& model = *mptr.lock();
                if (model.DATA.shadow == false) continue;
                gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
                VS.shad_static->set_mat<mat4>("matrix", light.sSky->matArr[csm] * model.matrix);
                model.mesh->bind_vao();
                for (uint i = 0; i < model.skin->mtrlVec.size(); i++) {
                    if (model.skin->mtrlVec[i].punch == true)
                        pipeline->use_shader(*FS.shad_punch),
                        model.skin->bind_textures(i, 1, 0, 0);
                    else pipeline->disable_stages(gl::FRAGMENT_SHADER_BIT);
                    model.mesh->draw_ibo(i);
                }
            }

            pipeline->use_shader(*VS.shad_skelly);
            for (const auto& mptr : modelSkellyList) {
                const wcoe::ModelSkelly& model = *mptr.lock();
                if (model.DATA.shadow == false) continue;
                gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
                VS.shad_skelly->set_mat<mat4>("matrix", light.sSky->matArr[csm] * model.matrix);
                VS.shad_skelly->set_vecptr<vec4>("skelQuat", (float*)model.skel->quatVec.data());
                VS.shad_skelly->set_vecptr<vec3>("skelOffs", (float*)model.skel->offsVec.data());
                model.mesh->bind_vao();
                for (uint i = 0; i < model.skin->mtrlVec.size(); i++) {
                    if (model.skin->mtrlVec[i].punch == true)
                        pipeline->use_shader(*FS.shad_punch),
                        model.skin->bind_textures(i, 1, 0, 0);
                    else pipeline->disable_stages(gl::FRAGMENT_SHADER_BIT);
                    model.mesh->draw_ibo(i);
                }
            }
        }
        gl::Disable(gl::DEPTH_CLAMP);
    }
}


void Graph::render_shadows_spot() {
    for (const auto& lptr : lightSpotList) {
        const wcoe::LightSpot& light = *lptr.lock();
        if (light.DATA.shadow == false) continue;
        if (sq::frus_in_frus(light.frus, camera->frus)) continue;

        light.sSpot->ubo->bind(1);
        gl::Viewport(0, 0, light.sSpot->tex->size.x, light.sSpot->tex->size.y);
        light.sSpot->fbo->use();
        gl::Clear(gl::DEPTH_BUFFER_BIT);

        pipeline->use_shader(*VS.shad_static);
        for (const auto& mptr : modelStaticList) {
            const wcoe::ModelStatic& model = *mptr.lock();
            if (model.DATA.shadow == false) continue;
            if (sq::bbox_in_frus(model.bbox, light.frus)) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_static->set_mat<mat4>("matrix", light.sSpot->matrix * model.matrix);
            model.mesh->bind_vao();
            for (uint i = 0; i < model.skin->mtrlVec.size(); i++) {
                if (model.skin->mtrlVec[i].punch == true)
                    pipeline->use_shader(*FS.shad_punch),
                    model.skin->bind_textures(i, 1, 0, 0);
                else pipeline->disable_stages(gl::FRAGMENT_SHADER_BIT);
                model.mesh->draw_ibo(i);
            }
        }

        pipeline->use_shader(*VS.shad_skelly);
        for (const auto& mptr : modelSkellyList) {
            const wcoe::ModelSkelly& model = *mptr.lock();
            if (model.DATA.shadow == false) continue;
            if (sq::bbox_in_frus(model.bbox, light.frus)) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_skelly->set_mat<mat4>("matrix", light.sSpot->matrix * model.matrix);
            VS.shad_skelly->set_vecptr<vec4>("skelQuat", (float*)model.skel->quatVec.data());
            VS.shad_skelly->set_vecptr<vec3>("skelOffs", (float*)model.skel->offsVec.data());
            model.mesh->bind_vao();
            for (uint i = 0; i < model.skin->mtrlVec.size(); i++) {
                if (model.skin->mtrlVec[i].punch == true)
                    pipeline->use_shader(*FS.shad_punch),
                    model.skin->bind_textures(i, 1, 0, 0);
                else pipeline->disable_stages(gl::FRAGMENT_SHADER_BIT);
                model.mesh->draw_ibo(i);
            }
        }
    }
}


void Graph::render_shadows_point() {
    for (const auto& lptr : lightPointList) {
        const wcoe::LightPoint& light = *lptr.lock();
        if (light.DATA.shadow == false) continue;
        if (sq::sphr_in_frus(light.sphere, camera->frus)) continue;

        light.sPoint->ubo->bind(1);
        gl::Viewport(0, 0, light.sPoint->tex->size, light.sPoint->tex->size);
        for (int face = 0; face < 6; face++) {
            if (sq::frus_in_frus(light.frusArr[face], camera->frus)) continue;
            light.sPoint->fboArr[face]->use();
            gl::Clear(gl::DEPTH_BUFFER_BIT);

            pipeline->use_shader(*VS.shad_static);
            for (const auto& mptr : modelStaticList) {
                const wcoe::ModelStatic& model = *mptr.lock();
                if (model.DATA.shadow == false) continue;
                if (sq::bbox_in_frus(model.bbox, light.frusArr[face])) continue;
                gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
                VS.shad_static->set_mat<mat4>("matrix", light.sPoint->matArr[face] * model.matrix);
                model.mesh->bind_vao();
                for (uint i = 0; i < model.skin->mtrlVec.size(); i++) {
                    if (model.skin->mtrlVec[i].punch == true)
                        pipeline->use_shader(*FS.shad_punch),
                        model.skin->bind_textures(i, 1, 0, 0);
                    else pipeline->disable_stages(gl::FRAGMENT_SHADER_BIT);
                    model.mesh->draw_ibo(i);
                }
            }

            pipeline->use_shader(*VS.shad_skelly);
            for (const auto& mptr : modelSkellyList) {
                const wcoe::ModelSkelly& model = *mptr.lock();
                if (model.DATA.shadow == false) continue;
                if (sq::bbox_in_frus(model.bbox, light.frusArr[face])) continue;
                gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
                VS.shad_skelly->set_mat<mat4>("matrix", light.sPoint->matArr[face] * model.matrix);
                VS.shad_skelly->set_vecptr<vec4>("skelQuat", (float*)model.skel->quatVec.data());
                VS.shad_skelly->set_vecptr<vec3>("skelOffs", (float*)model.skel->offsVec.data());
                model.mesh->bind_vao();
                for (uint i = 0; i < model.skin->mtrlVec.size(); i++) {
                    if (model.skin->mtrlVec[i].punch == true)
                        pipeline->use_shader(*FS.shad_punch),
                        model.skin->bind_textures(i, 1, 0, 0);
                    else pipeline->disable_stages(gl::FRAGMENT_SHADER_BIT);
                    model.mesh->draw_ibo(i);
                }
            }
        }
        gl::Disable(gl::DEPTH_CLAMP);
    }
}


void Graph::render_lights_sky() {
    for (const auto& lptr : lightSkyList) {
        const wcoe::LightSky& light = *lptr.lock();
        int diff_spec = 1 * light.DATA.diffuse | 2 * light.DATA.specular;
        if (light.DATA.shadow) {
            pipeline->use_shader(*FS.shds_skylight_shad);
            FS.shds_skylight_shad->set_sca<int>("diff_spec", diff_spec);
            light.sSky->ubo->bind(1);
            light.sSky->tex->bind(gl::TEXTURE6);
        } else {
            pipeline->use_shader(*FS.shds_skylight);
            FS.shds_skylight->set_sca<int>("diff_spec", diff_spec);
            light.lSky->ubo->bind(1);
        } sq::draw_screen_quad();
    }
}


void Graph::render_lights_spot() {
    for (const auto& lptr : lightSpotList) {
        const wcoe::LightSpot& light = *lptr.lock();
        if (sq::frus_in_frus(light.frus, camera->frus)) continue;
        int diff_spec = 1 * light.DATA.diffuse | 2 * light.DATA.specular;
        if (light.DATA.shadow) {
            pipeline->use_shader(*FS.shds_spotlight_shad);
            FS.shds_spotlight_shad->set_sca<int>("diff_spec", diff_spec);
            light.sSpot->ubo->bind(1);
            light.sSpot->tex->bind(gl::TEXTURE6);
        } else {
            pipeline->use_shader(*FS.shds_spotlight);
            FS.shds_spotlight->set_sca<int>("diff_spec", diff_spec);
            light.lSpot->ubo->bind(1);
        } sq::draw_screen_quad();
    }
}


void Graph::render_lights_point() {
    for (const auto& lptr : lightPointList) {
        const wcoe::LightPoint& light = *lptr.lock();
        if (sq::sphr_in_frus(light.sphere, camera->frus)) continue;
        int diff_spec = 1 * light.DATA.diffuse | 2 * light.DATA.specular;
        if (light.DATA.shadow) {
            pipeline->use_shader(*FS.shds_pointlight_shad);
            FS.shds_pointlight_shad->set_sca<int>("diff_spec", diff_spec);
            light.sPoint->ubo->bind(1);
            light.sPoint->tex->bind(gl::TEXTURE6);
        } else {
            pipeline->use_shader(*FS.shds_pointlight);
            FS.shds_pointlight->set_sca<int>("diff_spec", diff_spec);
            light.lPoint->ubo->bind(1);
        } sq::draw_screen_quad();
    }
}
