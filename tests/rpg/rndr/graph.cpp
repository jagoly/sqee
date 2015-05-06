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

#include "../main/camera.hpp"
#include "../wcoe/world.hpp"
#include "../wcoe/obj/modelstatic.hpp"
#include "../wcoe/obj/modelskelly.hpp"
#include "../wcoe/obj/spotlight.hpp"
#include "../wcoe/obj/pointlight.hpp"
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

    for (auto iter = spotLightList.begin(); iter != spotLightList.end(); iter++) {
        if (iter->expired()) { spotLightList.erase(iter--); continue; }
        const wcoe::SpotLight& obj = *iter->lock();
    }

    for (auto iter = pointLightList.begin(); iter != pointLightList.end(); iter++) {
        if (iter->expired()) { pointLightList.erase(iter--); continue; }
        const wcoe::PointLight& obj = *iter->lock();
    }
}


void Graph::update_settings() {
    int shadQlty = settings->crnt<int>("shadQlty");
    int shadFltr = settings->crnt<int>("shadFltr");

    uint adjSize = 1024u * std::pow(2, shadQlty);
    world->skylight.tex->resize(uvec3(adjSize, adjSize, 4u));
    if (shadFltr > 0) world->skylight.tex->set_preset(sq::Texture2DArray::L_C());
    else world->skylight.tex->set_preset(sq::Texture2DArray::N_C());

    for (const auto& lptr : spotLightList) {
        const wcoe::SpotLight& light = *lptr.lock();
        if (light.DAT_shadow == true) {
            adjSize = light.DAT_texsize * std::pow(2, shadQlty);
            light.tex->resize(uvec2(adjSize, adjSize));
            if (shadFltr > 0) light.tex->set_preset(sq::Texture2D::L_C());
            else light.tex->set_preset(sq::Texture2D::N_C());
        }
    }
    for (const auto& lptr : pointLightList) {
        const wcoe::PointLight& light = *lptr.lock();
        if (light.DAT_shadow == true) {
            adjSize = light.DAT_texsize * std::pow(2, shadQlty);
            light.tex->resize(adjSize);
            if (shadFltr > 0) light.tex->set_preset(sq::TextureCube::L_C());
            else light.tex->set_preset(sq::TextureCube::N_C());
        }
    }
}


void Graph::update_from_world() {
    modelStaticList.clear();
    modelSkellyList.clear();
    spotLightList.clear();
    pointLightList.clear();
    for (const auto& model : world->filtered<wcoe::ModelStatic>())
        modelStaticList.push_back(model);
    for (const auto& model : world->filtered<wcoe::ModelSkelly>())
        modelSkellyList.emplace_back(model);
    for (const auto& light : world->filtered<wcoe::SpotLight>())
        spotLightList.emplace_back(light);
    for (const auto& light : world->filtered<wcoe::PointLight>())
        pointLightList.emplace_back(light);

    update(); update_settings();
}


void Graph::render_models() {
    pipeline->use_shader(*FS.modl_write);

    pipeline->use_shader(*VS.modl_static);
    for (const auto& mptr : modelStaticList) {
        const wcoe::ModelStatic& model = *mptr.lock();
        if (model.DAT_render == false) continue;
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
        if (model.DAT_render == false) continue;
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
    if (!world->skylight.enabled) return;
    const wcoe::SkyLight& light = world->skylight;

    light.ubo->bind(1);
    gl::Viewport(0, 0, light.tex->size.x, light.tex->size.y);
    gl::Enable(gl::DEPTH_CLAMP);
    for (int csm = 0; csm < 4; csm++) {
        light.fboArr[csm]->use();
        gl::Clear(gl::DEPTH_BUFFER_BIT);

        pipeline->use_shader(*VS.shad_static);
        for (const auto& mptr : modelStaticList) {
            const wcoe::ModelStatic& model = *mptr.lock();
            if (model.DAT_shadow == false) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_static->set_mat<mat4>("matrix", light.matArr[csm] * model.matrix);
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
            if (model.DAT_shadow == false) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_skelly->set_mat<mat4>("matrix", light.matArr[csm] * model.matrix);
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


void Graph::render_shadows_spot() {
    for (const auto& lptr : spotLightList) {
        const wcoe::SpotLight& light = *lptr.lock();
        if (light.DAT_shadow == false) continue;
        if (sq::frus_in_frus(light.frus, camera->frus)) continue;

        light.ubo->bind(1); light.fbo->use();
        gl::Viewport(0, 0, light.tex->size.x, light.tex->size.y);
        gl::Clear(gl::DEPTH_BUFFER_BIT);

        pipeline->use_shader(*VS.shad_static);
        for (const auto& mptr : modelStaticList) {
            const wcoe::ModelStatic& model = *mptr.lock();
            if (model.DAT_shadow == false) continue;
            if (sq::bbox_in_frus(model.bbox, light.frus)) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_static->set_mat<mat4>("matrix", light.matrix * model.matrix);
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
            if (model.DAT_shadow == false) continue;
            if (sq::bbox_in_frus(model.bbox, light.frus)) continue;
            gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
            VS.shad_skelly->set_mat<mat4>("matrix", light.matrix * model.matrix);
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
    for (const auto& lptr : pointLightList) {
        const wcoe::PointLight& light = *lptr.lock();
        if (light.DAT_shadow == false) continue;
        if (sq::sphr_in_frus(light.sphere, camera->frus)) continue;

        light.ubo->bind(1);
        gl::Viewport(0, 0, light.tex->size, light.tex->size);
        for (int face = 0; face < 6; face++) {
            if (sq::frus_in_frus(light.frusArr[face], camera->frus)) continue;

            light.fboArr[face]->use();
            gl::Clear(gl::DEPTH_BUFFER_BIT);

            pipeline->use_shader(*VS.shad_static);
            for (const auto& mptr : modelStaticList) {
                const wcoe::ModelStatic& model = *mptr.lock();
                if (model.DAT_shadow == false) continue;
                if (sq::bbox_in_frus(model.bbox, light.frusArr[face])) continue;
                gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
                VS.shad_static->set_mat<mat4>("matrix", light.matArr[face] * model.matrix);
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
                if (model.DAT_shadow == false) continue;
                if (sq::bbox_in_frus(model.bbox, light.frusArr[face])) continue;
                gl::FrontFace(model.negScale ? gl::CW : gl::CCW);
                VS.shad_skelly->set_mat<mat4>("matrix", light.matArr[face] * model.matrix);
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


void Graph::render_skybox() {
    if (!world->skybox.enabled) return;
    pipeline->use_shader(*VS.shds_skybox);
    pipeline->use_shader(*FS.shds_skybox);
    world->skybox.tex->bind(gl::TEXTURE0);
    sq::draw_screen_quad();
}


void Graph::render_ambient() {
    if (!world->ambient.enabled) return;
    pipeline->use_shader(*VS.gnrc_quad);
    pipeline->use_shader(*FS.shds_ambient);
    world->ambient.ubo->bind(1);
    sq::draw_screen_quad();
}


void Graph::render_skylight() {
    if (!world->skylight.enabled) return;
    pipeline->use_shader(*VS.gnrc_quad);
    pipeline->use_shader(*FS.shds_skylight);
    world->skylight.tex->bind(gl::TEXTURE6);
    world->skylight.ubo->bind(1);
    sq::draw_screen_quad();
}


void Graph::render_spotlights() {
    pipeline->use_shader(*VS.gnrc_quad);
    for (const auto& lptr : spotLightList) {
        const wcoe::SpotLight& light = *lptr.lock();
        if (sq::frus_in_frus(light.frus, camera->frus)) continue;
        if (!light.DAT_shadow && !light.DAT_specular) pipeline->use_shader(*FS.shds_spot_none);
        if ( light.DAT_shadow && !light.DAT_specular) pipeline->use_shader(*FS.shds_spot_shad);
        if (!light.DAT_shadow &&  light.DAT_specular) pipeline->use_shader(*FS.shds_spot_spec);
        if ( light.DAT_shadow &&  light.DAT_specular) pipeline->use_shader(*FS.shds_spot_both);
        if (light.DAT_shadow) light.tex->bind(gl::TEXTURE6);
        light.ubo->bind(1); sq::draw_screen_quad();
    }
}


void Graph::render_pointlights() {
    pipeline->use_shader(*VS.gnrc_quad);
    for (const auto& lptr : pointLightList) {
        const wcoe::PointLight& light = *lptr.lock();
        if (sq::sphr_in_frus(light.sphere, camera->frus)) continue;
        if (!light.DAT_shadow && !light.DAT_specular) pipeline->use_shader(*FS.shds_point_none);
        if ( light.DAT_shadow && !light.DAT_specular) pipeline->use_shader(*FS.shds_point_shad);
        if (!light.DAT_shadow &&  light.DAT_specular) pipeline->use_shader(*FS.shds_point_spec);
        if ( light.DAT_shadow &&  light.DAT_specular) pipeline->use_shader(*FS.shds_point_both);
        if (light.DAT_shadow) light.tex->bind(gl::TEXTURE6);
        light.ubo->bind(1); sq::draw_screen_quad();
    }
}
