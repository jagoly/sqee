#include <glm/matrix.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/app/logging.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/shaders.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/models/modelstatic.hpp>
#include <sqee/models/modelskelly.hpp>
#include <sqee/render/skylight.hpp>
#include <sqee/render/spotlight.hpp>
#include <sqee/render/pointlight.hpp>

#include "camera.hpp"
#include "graph.hpp"

using namespace sqt;

Graph::Graph() {}

void Graph::update() {
    for (auto iter = staticList.begin(); iter != staticList.end(); iter++) {
        if (iter->expired()) { staticList.erase(iter--); continue; }
        const sq::ModelStatic& obj = *iter->lock();
    }

    for (auto iter = skellyList.begin(); iter != skellyList.end(); iter++) {
        if (iter->expired()) { skellyList.erase(iter--); continue; }
        const sq::ModelSkelly& obj = *iter->lock();
    }

    for (auto iter = skyLightList.begin(); iter != skyLightList.end(); iter++) {
        if (iter->expired()) { skyLightList.erase(iter--); continue; }
        const sq::SkyLight& obj = *iter->lock();
    }

    for (auto iter = spotLightList.begin(); iter != spotLightList.end(); iter++) {
        if (iter->expired()) { spotLightList.erase(iter--); continue; }
        const sq::SpotLight& obj = *iter->lock();
    }

    for (auto iter = pointLightList.begin(); iter != pointLightList.end(); iter++) {
        if (iter->expired()) { pointLightList.erase(iter--); continue; }
        const sq::PointLight& obj = *iter->lock();
    }
}


void Graph::render_models() {
    bool negSca = false; gl::FrontFace(gl::CCW);
    pipeline->use_shader(*FS.modl_write);

    pipeline->use_shader(*VS.modl_static);
    for (const auto& mptr : staticList) {
        const sq::ModelStatic& model = *mptr.lock();
        if (sq::bbox_in_frus(model.bbox, camera->frus)) continue;
        if ((glm::determinant(model.matrix) < 0.f) == !negSca)
            gl::FrontFace((negSca = !negSca) ? gl::CW : gl::CCW);
        VS.modl_static->set_mat<mat4>("modelMat", model.matrix);
        VS.modl_static->set_mat<mat3>("normMat", sq::make_normMat(camera->viewMat * model.matrix));
        model.mesh->bind_vao();
        for (uint i = 0u; i < model.skin->mtrlVec.size(); i++)
            FS.modl_write->set_sca<int>("mode", model.skin->mtrlVec[i].glMode),
            model.skin->bind_textures(i),
            model.mesh->draw_ibo(i);
    }

    pipeline->use_shader(*VS.modl_skelly);
    for (const auto& mptr : skellyList) {
        const sq::ModelSkelly& model = *mptr.lock();
        if (sq::bbox_in_frus(model.bbox, camera->frus)) continue;
        if ((glm::determinant(model.matrix) < 0.f) == !negSca)
            gl::FrontFace((negSca = !negSca) ? gl::CW : gl::CCW);
        VS.modl_skelly->set_mat<mat4>("modelMat", model.matrix);
        VS.modl_skelly->set_mat<mat3>("normMat", sq::make_normMat(camera->viewMat * model.matrix));
        VS.modl_skelly->set_vecptr<vec4>("skelQuat", (float*)model.skel.quatVec.data());
        VS.modl_skelly->set_vecptr<vec3>("skelOffs", (float*)model.skel.offsVec.data());
        model.mesh->bind_vao();
        for (uint i = 0u; i < model.skin->mtrlVec.size(); i++)
            FS.modl_write->set_sca<int>("mode", model.skin->mtrlVec[i].glMode),
            model.skin->bind_textures(i),
            model.mesh->draw_ibo(i);
    }
}


void Graph::render_shadows_sky() {
    bool negSca = false; gl::FrontFace(gl::CCW);

    for (const auto& lptr : skyLightList) {
        const sq::SkyLight& light = *lptr.lock();

        light.ubo->bind(1);
        gl::Viewport(0, 0, light.tex->size.x, light.tex->size.y);
        gl::Enable(gl::DEPTH_CLAMP);
        for (int csm = 0; csm < 4; csm++) {
            light.fboArr[csm]->use();
            gl::Clear(gl::DEPTH_BUFFER_BIT);

            pipeline->disable_stages(gl::FRAGMENT_SHADER_BIT);

            pipeline->use_shader(*VS.shad_static_punch);
            for (const auto& mptr : staticList) {
                const sq::ModelStatic& model = *mptr.lock();
                auto fList = model.skin->filtered(0, 1); if (fList.empty()) continue;
                if ((glm::determinant(model.matrix) < 0.f) == !negSca)
                    gl::FrontFace((negSca = !negSca) ? gl::CW : gl::CCW);
                VS.shad_static_punch->set_mat<mat4>("matrix", light.matArr[csm] * model.matrix);
                model.mesh->bind_vao();
                for (auto& i : fList)
                    model.mesh->draw_ibo(i);
            }

            pipeline->use_shader(*VS.shad_skelly_punch);
            for (const auto& mptr : skellyList) {
                const sq::ModelSkelly& model = *mptr.lock();
                auto fList = model.skin->filtered(0, 1); if (fList.empty()) continue;
                if ((glm::determinant(model.matrix) < 0.f) == !negSca)
                    gl::FrontFace((negSca = !negSca) ? gl::CW : gl::CCW);
                VS.shad_skelly_punch->set_mat<mat4>("matrix", light.matArr[csm] * model.matrix);
                VS.shad_skelly_punch->set_vecptr<vec4>("skelQuat", (float*)model.skel.quatVec.data());
                VS.shad_skelly_punch->set_vecptr<vec3>("skelOffs", (float*)model.skel.offsVec.data());
                model.mesh->bind_vao();
                for (auto& i : fList)
                    model.mesh->draw_ibo(i);
            }

            pipeline->use_shader(*FS.shad_punch);

            pipeline->use_shader(*VS.shad_static_punch);
            for (const auto& mptr : staticList) {
                const sq::ModelStatic& model = *mptr.lock();
                auto fList = model.skin->filtered(1, 1); if (fList.empty()) continue;
                if ((glm::determinant(model.matrix) < 0.f) == !negSca)
                    gl::FrontFace((negSca = !negSca) ? gl::CW : gl::CCW);
                VS.shad_static_punch->set_mat<mat4>("matrix", light.matArr[csm] * model.matrix);
                model.mesh->bind_vao();
                for (auto& i : fList)
                    model.skin->bind_textures(i, 1, 0, 0),
                    model.mesh->draw_ibo(i);
            }

            pipeline->use_shader(*VS.shad_skelly_punch);
            for (const auto& mptr : skellyList) {
                const sq::ModelSkelly& model = *mptr.lock();
                auto fList = model.skin->filtered(1, 1); if (fList.empty()) continue;
                if ((glm::determinant(model.matrix) < 0.f) == !negSca)
                    gl::FrontFace((negSca = !negSca) ? gl::CW : gl::CCW);
                VS.shad_skelly_punch->set_mat<mat4>("matrix", light.matArr[csm] * model.matrix);
                VS.shad_skelly_punch->set_vecptr<vec4>("skelQuat", (float*)model.skel.quatVec.data());
                VS.shad_skelly_punch->set_vecptr<vec3>("skelOffs", (float*)model.skel.offsVec.data());
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
    bool negSca = false; gl::FrontFace(gl::CCW);

    for (const auto& lptr : spotLightList) {
        const sq::SpotLight& light = *lptr.lock();
        if (sq::frus_in_frus(light.frus, camera->frus)) continue;

        light.ubo->bind(1);
        gl::Viewport(0, 0, light.tex->size.x, light.tex->size.y);
        light.fbo->use();
        gl::Clear(gl::DEPTH_BUFFER_BIT);

        pipeline->disable_stages(gl::FRAGMENT_SHADER_BIT);

        pipeline->use_shader(*VS.shad_static_punch);
        for (const auto& mptr : staticList) {
            const sq::ModelStatic& model = *mptr.lock();
            auto fList = model.skin->filtered(0, 1); if (fList.empty()) continue;
            if (sq::bbox_in_frus(model.bbox, light.frus)) continue;
            if ((glm::determinant(model.matrix) < 0.f) == !negSca)
                gl::FrontFace((negSca = !negSca) ? gl::CW : gl::CCW);
            VS.shad_static_punch->set_mat<mat4>("matrix", light.matrix * model.matrix);
            model.mesh->bind_vao();
            for (auto& i : fList)
                model.mesh->draw_ibo(i);
        }

        pipeline->use_shader(*VS.shad_skelly_punch);
        for (const auto& mptr : skellyList) {
            const sq::ModelSkelly& model = *mptr.lock();
            auto fList = model.skin->filtered(0, 1); if (fList.empty()) continue;
            if (sq::bbox_in_frus(model.bbox, light.frus)) continue;
            if ((glm::determinant(model.matrix) < 0.f) == !negSca)
                gl::FrontFace((negSca = !negSca) ? gl::CW : gl::CCW);
            VS.shad_skelly_punch->set_mat<mat4>("matrix", light.matrix * model.matrix);
            VS.shad_skelly_punch->set_vecptr<vec4>("skelQuat", (float*)model.skel.quatVec.data());
            VS.shad_skelly_punch->set_vecptr<vec3>("skelOffs", (float*)model.skel.offsVec.data());
            model.mesh->bind_vao();
            for (auto& i : fList)
                model.mesh->draw_ibo(i);
        }

        pipeline->use_shader(*FS.shad_punch);

        pipeline->use_shader(*VS.shad_static_punch);
        for (const auto& mptr : staticList) {
            const sq::ModelStatic& model = *mptr.lock();
            auto fList = model.skin->filtered(1, 1); if (fList.empty()) continue;
            if (sq::bbox_in_frus(model.bbox, light.frus)) continue;
            if ((glm::determinant(model.matrix) < 0.f) == !negSca)
                gl::FrontFace((negSca = !negSca) ? gl::CW : gl::CCW);
            VS.shad_static_punch->set_mat<mat4>("matrix", light.matrix * model.matrix);
            model.mesh->bind_vao();
            for (auto& i : fList)
                model.skin->bind_textures(i, 1, 0, 0),
                model.mesh->draw_ibo(i);
        }

        pipeline->use_shader(*VS.shad_skelly_punch);
        for (const auto& mptr : skellyList) {
            const sq::ModelSkelly& model = *mptr.lock();
            auto fList = model.skin->filtered(1, 1); if (fList.empty()) continue;
            if (sq::bbox_in_frus(model.bbox, light.frus)) continue;
            if ((glm::determinant(model.matrix) < 0.f) == !negSca)
                gl::FrontFace((negSca = !negSca) ? gl::CW : gl::CCW);
            VS.shad_skelly_punch->set_mat<mat4>("matrix", light.matrix * model.matrix);
            VS.shad_skelly_punch->set_vecptr<vec4>("skelQuat", (float*)model.skel.quatVec.data());
            VS.shad_skelly_punch->set_vecptr<vec3>("skelOffs", (float*)model.skel.offsVec.data());
            model.mesh->bind_vao();
            for (auto& i : fList)
                model.skin->bind_textures(i, 1, 0, 0),
                model.mesh->draw_ibo(i);
        }
    }
}


void Graph::render_shadows_point() {
    bool negSca = false; gl::FrontFace(gl::CCW);

    for (const auto& lptr : pointLightList) {
        const sq::PointLight& light = *lptr.lock();
        if (sq::sphere_in_frus(vec4(light.position, light.intensity), camera->frus)) continue;

        light.ubo->bind(1);
        gl::Viewport(0, 0, light.tex->size, light.tex->size);
        for (int face = 0; face < 6; face++) {
            if (sq::frus_in_frus(light.frusArr[face], camera->frus)) continue;
            light.fboArr[face]->use();
            gl::Clear(gl::DEPTH_BUFFER_BIT);

            pipeline->disable_stages(gl::FRAGMENT_SHADER_BIT);

            pipeline->use_shader(*VS.shad_static);
            for (const auto& mptr : staticList) {
                const sq::ModelStatic& model = *mptr.lock();
                auto fList = model.skin->filtered(0, 1); if (fList.empty()) continue;
                if (sq::bbox_in_frus(model.bbox, light.frusArr[face])) continue;
                if ((glm::determinant(model.matrix) < 0.f) == !negSca)
                    gl::FrontFace((negSca = !negSca) ? gl::CW : gl::CCW);
                VS.shad_static->set_mat<mat4>("matrix", light.matArr[face] * model.matrix);
                model.mesh->bind_vao();
                for (auto& i : fList)
                    model.mesh->draw_ibo(i);
            }

            pipeline->use_shader(*VS.shad_skelly);
            for (const auto& mptr : skellyList) {
                const sq::ModelSkelly& model = *mptr.lock();
                auto fList = model.skin->filtered(0, 1); if (fList.empty()) continue;
                if (sq::bbox_in_frus(model.bbox, light.frusArr[face])) continue;
                if ((glm::determinant(model.matrix) < 0.f) == !negSca)
                    gl::FrontFace((negSca = !negSca) ? gl::CW : gl::CCW);
                VS.shad_skelly->set_mat<mat4>("matrix", light.matArr[face] * model.matrix);
                VS.shad_skelly->set_vecptr<vec4>("skelQuat", (float*)model.skel.quatVec.data());
                VS.shad_skelly->set_vecptr<vec3>("skelOffs", (float*)model.skel.offsVec.data());
                model.mesh->bind_vao();
                for (auto& i : fList)
                    model.mesh->draw_ibo(i);
            }

            pipeline->use_shader(*FS.shad_punch);

            pipeline->use_shader(*VS.shad_static_punch);
            for (const auto& mptr : staticList) {
                const sq::ModelStatic& model = *mptr.lock();
                auto fList = model.skin->filtered(1, 1); if (fList.empty()) continue;
                if (sq::bbox_in_frus(model.bbox, light.frusArr[face])) continue;
                if ((glm::determinant(model.matrix) < 0.f) == !negSca)
                    gl::FrontFace((negSca = !negSca) ? gl::CW : gl::CCW);
                VS.shad_static_punch->set_mat<mat4>("matrix", light.matArr[face] * model.matrix);
                model.mesh->bind_vao();
                for (auto& i : fList)
                    model.skin->bind_textures(i, 1, 0, 0),
                    model.mesh->draw_ibo(i);
            }

            pipeline->use_shader(*VS.shad_skelly_punch);
            for (const auto& mptr : skellyList) {
                const sq::ModelSkelly& model = *mptr.lock();
                auto fList = model.skin->filtered(1, 1); if (fList.empty()) continue;
                if (sq::bbox_in_frus(model.bbox, light.frusArr[face])) continue;
                if ((glm::determinant(model.matrix) < 0.f) == !negSca)
                    gl::FrontFace((negSca = !negSca) ? gl::CW : gl::CCW);
                VS.shad_skelly_punch->set_mat<mat4>("matrix", light.matArr[face] * model.matrix);
                VS.shad_skelly_punch->set_vecptr<vec4>("skelQuat", (float*)model.skel.quatVec.data());
                VS.shad_skelly_punch->set_vecptr<vec3>("skelOffs", (float*)model.skel.offsVec.data());
                model.mesh->bind_vao();
                for (auto& i : fList)
                    model.skin->bind_textures(i, 1, 0, 0),
                    model.mesh->draw_ibo(i);
            }
        }
        gl::Disable(gl::DEPTH_CLAMP);
    }
}
