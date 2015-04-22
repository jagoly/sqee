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
        light.sky->resize_texture(settings->crnt<int>("shadQuality"));
        light.sky->filter_texture(settings->crnt<int>("shadFilter"));
    }
    for (const auto& lptr : lightSpotList) {
        const wcoe::LightSpot& light = *lptr.lock();
        light.spot->resize_texture(settings->crnt<int>("shadQuality"));
        light.spot->filter_texture(settings->crnt<int>("shadFilter"));
    }
    for (const auto& lptr : lightPointList) {
        const wcoe::LightPoint& light = *lptr.lock();
        light.point->resize_texture(settings->crnt<int>("shadQuality"));
        light.point->filter_texture(settings->crnt<int>("shadFilter"));
    }
}


void Graph::render_models() {
    pipeline->use_shader(*FS.modl_write);

    pipeline->use_shader(*VS.modl_static);
    for (const auto& mptr : modelStaticList) {
        const wcoe::ModelStatic& model = *mptr.lock();
        if (sq::bbox_in_frus(model.bbox, camera->frus)) continue;
        gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
        VS.modl_static->set_mat<mat4>("modelMat", model.matrix);
        VS.modl_static->set_mat<mat3>("normMat", sq::make_normMat(camera->viewMat * model.matrix));
        model.mesh->bind_vao();
        for (uint i = 0u; i < model.skin->mtrlVec.size(); i++)
            FS.modl_write->set_sca<int>("mode", model.skin->mtrlVec[i].glMode),
            model.skin->bind_textures(i),
            model.mesh->draw_ibo(i);
    }

    pipeline->use_shader(*VS.modl_skelly);
    for (const auto& mptr : modelSkellyList) {
        const wcoe::ModelSkelly& model = *mptr.lock();
        if (sq::bbox_in_frus(model.bbox, camera->frus)) continue;
        gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
        VS.modl_skelly->set_mat<mat4>("modelMat", model.matrix);
        VS.modl_skelly->set_mat<mat3>("normMat", sq::make_normMat(camera->viewMat * model.matrix));
        VS.modl_skelly->set_vecptr<vec4>("skelQuat", (float*)model.skel->quatVec.data());
        VS.modl_skelly->set_vecptr<vec3>("skelOffs", (float*)model.skel->offsVec.data());
        model.mesh->bind_vao();
        for (uint i = 0u; i < model.skin->mtrlVec.size(); i++)
            FS.modl_write->set_sca<int>("mode", model.skin->mtrlVec[i].glMode),
            model.skin->bind_textures(i),
            model.mesh->draw_ibo(i);
    }
}


void Graph::render_shadows_sky() {
    for (const auto& lptr : lightSkyList) {
        const wcoe::LightSky& light = *lptr.lock();

        light.sky->ubo->bind(1);
        gl::Viewport(0, 0, light.sky->tex->size.x, light.sky->tex->size.y);
        gl::Enable(gl::DEPTH_CLAMP);
        for (int csm = 0; csm < 4; csm++) {
            light.sky->fboArr[csm]->use();
            gl::Clear(gl::DEPTH_BUFFER_BIT);

            pipeline->disable_stages(gl::FRAGMENT_SHADER_BIT);

            pipeline->use_shader(*VS.shad_static_punch);
            for (const auto& mptr : modelStaticList) {
                const wcoe::ModelStatic& model = *mptr.lock();
                auto fList = model.skin->filtered(0, 1); if (fList.empty()) continue;
                gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
                VS.shad_static_punch->set_mat<mat4>("matrix", light.sky->matArr[csm] * model.matrix);
                model.mesh->bind_vao();
                for (auto& i : fList)
                    model.mesh->draw_ibo(i);
            }

            pipeline->use_shader(*VS.shad_skelly_punch);
            for (const auto& mptr : modelSkellyList) {
                const wcoe::ModelSkelly& model = *mptr.lock();
                auto fList = model.skin->filtered(0, 1); if (fList.empty()) continue;
                gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
                VS.shad_skelly_punch->set_mat<mat4>("matrix", light.sky->matArr[csm] * model.matrix);
                VS.shad_skelly_punch->set_vecptr<vec4>("skelQuat", (float*)model.skel->quatVec.data());
                VS.shad_skelly_punch->set_vecptr<vec3>("skelOffs", (float*)model.skel->offsVec.data());
                model.mesh->bind_vao();
                for (auto& i : fList)
                    model.mesh->draw_ibo(i);
            }

            pipeline->use_shader(*FS.shad_punch);

            pipeline->use_shader(*VS.shad_static_punch);
            for (const auto& mptr : modelStaticList) {
                const wcoe::ModelStatic& model = *mptr.lock();
                auto fList = model.skin->filtered(1, 1); if (fList.empty()) continue;
                gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
                VS.shad_static_punch->set_mat<mat4>("matrix", light.sky->matArr[csm] * model.matrix);
                model.mesh->bind_vao();
                for (auto& i : fList)
                    model.skin->bind_textures(i, 1, 0, 0),
                    model.mesh->draw_ibo(i);
            }

            pipeline->use_shader(*VS.shad_skelly_punch);
            for (const auto& mptr : modelSkellyList) {
                const wcoe::ModelSkelly& model = *mptr.lock();
                auto fList = model.skin->filtered(1, 1); if (fList.empty()) continue;
                gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
                VS.shad_skelly_punch->set_mat<mat4>("matrix", light.sky->matArr[csm] * model.matrix);
                VS.shad_skelly_punch->set_vecptr<vec4>("skelQuat", (float*)model.skel->quatVec.data());
                VS.shad_skelly_punch->set_vecptr<vec3>("skelOffs", (float*)model.skel->offsVec.data());
                model.mesh->bind_vao();
                for (auto& i : fList)
                    model.skin->bind_textures(i, 1, 0, 0),
                    model.mesh->draw_ibo(i);
            }
        }
        gl::Disable(gl::DEPTH_CLAMP);
    }
}


void Graph::render_shadows_spot() {
    for (const auto& lptr : lightSpotList) {
        const wcoe::LightSpot& light = *lptr.lock();
        if (sq::frus_in_frus(light.spot->frus, camera->frus)) continue;

        light.spot->ubo->bind(1);
        gl::Viewport(0, 0, light.spot->tex->size.x, light.spot->tex->size.y);
        light.spot->fbo->use();
        gl::Clear(gl::DEPTH_BUFFER_BIT);

        pipeline->disable_stages(gl::FRAGMENT_SHADER_BIT);

        pipeline->use_shader(*VS.shad_static_punch);
        for (const auto& mptr : modelStaticList) {
            const wcoe::ModelStatic& model = *mptr.lock();
            auto fList = model.skin->filtered(0, 1); if (fList.empty()) continue;
            if (sq::bbox_in_frus(model.bbox, light.spot->frus)) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_static_punch->set_mat<mat4>("matrix", light.spot->matrix * model.matrix);
            model.mesh->bind_vao();
            for (auto& i : fList)
                model.mesh->draw_ibo(i);
        }

        pipeline->use_shader(*VS.shad_skelly_punch);
        for (const auto& mptr : modelSkellyList) {
            const wcoe::ModelSkelly& model = *mptr.lock();
            auto fList = model.skin->filtered(0, 1); if (fList.empty()) continue;
            if (sq::bbox_in_frus(model.bbox, light.spot->frus)) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_skelly_punch->set_mat<mat4>("matrix", light.spot->matrix * model.matrix);
            VS.shad_skelly_punch->set_vecptr<vec4>("skelQuat", (float*)model.skel->quatVec.data());
            VS.shad_skelly_punch->set_vecptr<vec3>("skelOffs", (float*)model.skel->offsVec.data());
            model.mesh->bind_vao();
            for (auto& i : fList)
                model.mesh->draw_ibo(i);
        }

        pipeline->use_shader(*FS.shad_punch);

        pipeline->use_shader(*VS.shad_static_punch);
        for (const auto& mptr : modelStaticList) {
            const wcoe::ModelStatic& model = *mptr.lock();
            auto fList = model.skin->filtered(1, 1); if (fList.empty()) continue;
            if (sq::bbox_in_frus(model.bbox, light.spot->frus)) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_static_punch->set_mat<mat4>("matrix", light.spot->matrix * model.matrix);
            model.mesh->bind_vao();
            for (auto& i : fList)
                model.skin->bind_textures(i, 1, 0, 0),
                model.mesh->draw_ibo(i);
        }

        pipeline->use_shader(*VS.shad_skelly_punch);
        for (const auto& mptr : modelSkellyList) {
            const wcoe::ModelSkelly& model = *mptr.lock();
            auto fList = model.skin->filtered(1, 1); if (fList.empty()) continue;
            if (sq::bbox_in_frus(model.bbox, light.spot->frus)) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_skelly_punch->set_mat<mat4>("matrix", light.spot->matrix * model.matrix);
            VS.shad_skelly_punch->set_vecptr<vec4>("skelQuat", (float*)model.skel->quatVec.data());
            VS.shad_skelly_punch->set_vecptr<vec3>("skelOffs", (float*)model.skel->offsVec.data());
            model.mesh->bind_vao();
            for (auto& i : fList)
                model.skin->bind_textures(i, 1, 0, 0),
                model.mesh->draw_ibo(i);
        }
    }
}


void Graph::render_shadows_point() {
    for (const auto& lptr : lightPointList) {
        const wcoe::LightPoint& light = *lptr.lock();
        if (sq::sphere_in_frus(vec4(light.point->position, light.point->intensity), camera->frus)) continue;

        light.point->ubo->bind(1);
        gl::Viewport(0, 0, light.point->tex->size, light.point->tex->size);
        for (int face = 0; face < 6; face++) {
            if (sq::frus_in_frus(light.point->frusArr[face], camera->frus)) continue;
            light.point->fboArr[face]->use();
            gl::Clear(gl::DEPTH_BUFFER_BIT);

            pipeline->disable_stages(gl::FRAGMENT_SHADER_BIT);

            pipeline->use_shader(*VS.shad_static);
            for (const auto& mptr : modelStaticList) {
                const wcoe::ModelStatic& model = *mptr.lock();
                auto fList = model.skin->filtered(0, 1); if (fList.empty()) continue;
                if (sq::bbox_in_frus(model.bbox, light.point->frusArr[face])) continue;
                gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
                VS.shad_static->set_mat<mat4>("matrix", light.point->matArr[face] * model.matrix);
                model.mesh->bind_vao();
                for (auto& i : fList)
                    model.mesh->draw_ibo(i);
            }

            pipeline->use_shader(*VS.shad_skelly);
            for (const auto& mptr : modelSkellyList) {
                const wcoe::ModelSkelly& model = *mptr.lock();
                auto fList = model.skin->filtered(0, 1); if (fList.empty()) continue;
                if (sq::bbox_in_frus(model.bbox, light.point->frusArr[face])) continue;
                gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
                VS.shad_skelly->set_mat<mat4>("matrix", light.point->matArr[face] * model.matrix);
                VS.shad_skelly->set_vecptr<vec4>("skelQuat", (float*)model.skel->quatVec.data());
                VS.shad_skelly->set_vecptr<vec3>("skelOffs", (float*)model.skel->offsVec.data());
                model.mesh->bind_vao();
                for (auto& i : fList)
                    model.mesh->draw_ibo(i);
            }

            pipeline->use_shader(*FS.shad_punch);

            pipeline->use_shader(*VS.shad_static_punch);
            for (const auto& mptr : modelStaticList) {
                const wcoe::ModelStatic& model = *mptr.lock();
                auto fList = model.skin->filtered(1, 1); if (fList.empty()) continue;
                if (sq::bbox_in_frus(model.bbox, light.point->frusArr[face])) continue;
                gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
                VS.shad_static_punch->set_mat<mat4>("matrix", light.point->matArr[face] * model.matrix);
                model.mesh->bind_vao();
                for (auto& i : fList)
                    model.skin->bind_textures(i, 1, 0, 0),
                    model.mesh->draw_ibo(i);
            }

            pipeline->use_shader(*VS.shad_skelly_punch);
            for (const auto& mptr : modelSkellyList) {
                const wcoe::ModelSkelly& model = *mptr.lock();
                auto fList = model.skin->filtered(1, 1); if (fList.empty()) continue;
                if (sq::bbox_in_frus(model.bbox, light.point->frusArr[face])) continue;
                gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
                VS.shad_skelly_punch->set_mat<mat4>("matrix", light.point->matArr[face] * model.matrix);
                VS.shad_skelly_punch->set_vecptr<vec4>("skelQuat", (float*)model.skel->quatVec.data());
                VS.shad_skelly_punch->set_vecptr<vec3>("skelOffs", (float*)model.skel->offsVec.data());
                model.mesh->bind_vao();
                for (auto& i : fList)
                    model.skin->bind_textures(i, 1, 0, 0),
                    model.mesh->draw_ibo(i);
            }
        }
        gl::Disable(gl::DEPTH_CLAMP);
    }
}


void Graph::render_lights_sky() {
    pipeline->use_shader(*FS.shds_skylight);
    for (const auto& lptr : lightSkyList) {
        const wcoe::LightSky& light = *lptr.lock();
        light.sky->ubo->bind(1);
        light.sky->tex->bind(gl::TEXTURE6);
        sq::draw_screen_quad();
    }
}


void Graph::render_lights_spot() {
    pipeline->use_shader(*FS.shds_spotlight);
    for (const auto& lptr : lightSpotList) {
        const wcoe::LightSpot& light = *lptr.lock();
        if (sq::frus_in_frus(light.spot->frus, camera->frus)) continue;
        light.spot->ubo->bind(1);
        light.spot->tex->bind(gl::TEXTURE6);
        sq::draw_screen_quad();
    }
}


void Graph::render_lights_point() {
    pipeline->use_shader(*FS.shds_pointlight);
    for (const auto& lptr : lightPointList) {
        const wcoe::LightPoint& light = *lptr.lock();
        if (sq::sphere_in_frus(vec4(light.point->position, light.point->intensity), camera->frus)) continue;
        light.point->ubo->bind(1);
        light.point->tex->bind(gl::TEXTURE6);
        sq::draw_screen_quad();
    }
}
