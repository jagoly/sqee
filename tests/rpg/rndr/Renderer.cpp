#include <set>
#include <algorithm>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/PreProcessor.hpp>
#include <sqee/app/Settings.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Shaders.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/render/Armature.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>
#include <sqee/maths/General.hpp>
#include <sqee/maths/Volumes.hpp>

#include "../wcoe/World.hpp"
#include "../wcoe/SkyBox.hpp"
#include "../wcoe/Ambient.hpp"
#include "../wcoe/SkyLight.hpp"
#include "../wcoe/objects/ModelSimple.hpp"
#include "../wcoe/objects/ModelSkelly.hpp"
#include "../wcoe/objects/PointLight.hpp"
#include "../wcoe/objects/SpotLight.hpp"
#include "../wcoe/objects/Reflector.hpp"
#include "../wcoe/objects/Emitter.hpp"
#include "../wcoe/objects/Liquid.hpp"
#include "../wcoe/objects/Decal.hpp"

#include "Shadows.hpp"
#include "Gbuffers.hpp"
#include "Lighting.hpp"
#include "Pretties.hpp"
#include "Reflects.hpp"
#include "Renderer.hpp"

using namespace sqt::rndr;
namespace maths = sq::maths;

Renderer::Renderer(const sq::Settings& _settings, const sq::PreProcessor& _preprocs,
                   const sq::Pipeline& _pipeline, const wcoe::World& _world)
    : settings(_settings), preprocs(_preprocs),
      pipeline(_pipeline), world(_world) {

    shadows.reset(new Shadows(*this));
    gbuffers.reset(new Gbuffers(*this));
    lighting.reset(new Lighting(*this));
    pretties.reset(new Pretties(*this));
    reflects.reset(new Reflects(*this));

    // Particles /////
    gl::GenVertexArrays(1, &partVAO);
    gl::GenBuffers(1, &partVBO);
    gl::GenBuffers(1, &partIBO);

    gl::BindVertexArray(partVAO);
    gl::BindBuffer(gl::ARRAY_BUFFER, partVBO);
    gl::VertexAttribPointer(0, 4, gl::FLOAT, false, 32, (void*)(0));
    gl::VertexAttribPointer(1, 4, gl::FLOAT, false, 32, (void*)(16));
    gl::EnableVertexAttribArray(0); gl::EnableVertexAttribArray(1);

    VS_stencil_base.add_uniform("matrix"); // mat4
    VS_stencil_refl.add_uniform("matrix"); // mat4

    preprocs.load(VS_fullscreen, "generic/fullscreen_vs");
    preprocs.load(VS_stencil_base, "generic/stencil_base_vs");
    preprocs.load(VS_stencil_refl, "generic/stencil_refl_vs");
    preprocs.load(FS_passthrough, "generic/passthrough_fs");

    preprocs.load(VS_part_soft_vertex, "particles/soft/vertex_vs");
    preprocs.load(GS_part_soft_geometry, "particles/soft/geometry_gs");
    preprocs.load(FS_part_soft_ambient, "particles/soft/ambient_fs");
    preprocs.load(FS_part_soft_write, "particles/soft/write_fs");
}


void Renderer::reload_lists() {
    modelSimpleVec = world.filtered<wcoe::ModelSimple>();
    modelSkellyVec = world.filtered<wcoe::ModelSkelly>();
    pointLightVec = world.filtered<wcoe::PointLight>();
    spotLightVec = world.filtered<wcoe::SpotLight>();
    reflectorVec = world.filtered<wcoe::Reflector>();
    emitterVec = world.filtered<wcoe::Emitter>();
    decalVec = world.filtered<wcoe::Decal>();

    update_settings();
}


template<class T_Container, class T_Compare>
inline void sort_container_by(T_Container& _container, const T_Compare _compare) {
    std::sort(_container.begin(), _container.end(), _compare); }

void Renderer::cull_and_sort() {
    cameraData = CameraData();
    skyLightData = SkyLightData();
    spotLightDataVec.clear();
    pointLightDataVec.clear();
    reflectorDataVec.clear();
    emitterDataVec.clear();

    std::set<const wcoe::SpotLight*> shadowSpotLightSet;
    std::set<const wcoe::PointLight*> shadowPointLightSet;
    array<std::set<const wcoe::PointLight*>, 6> shadowPLFaceSetArr;

    for (const wcoe::Reflector* rflct : reflectorVec) {
        if (rflct->PROP_shadow && maths::dot(world.skylight->PROP_direction, rflct->normal) < 0.f) {
            for (uint ind = 0u; ind < 4u; ++ind)
                if (sq::bbox_in_orth(rflct->bbox, world.skylight->orthArrA[ind]))
                    skyLightData.reflectorVecArrA[ind].push_back(rflct);
            for (uint ind = 0u; ind < 2u; ++ind)
                if (sq::bbox_in_orth(rflct->bbox, world.skylight->orthArrB[ind]))
                    skyLightData.reflectorVecArrB[ind].push_back(rflct);
        }

        if (maths::dot(camera->pos, rflct->normal) + rflct->offset > 0.f
            && sq::bbox_in_frus(rflct->bbox, camera->frus)) {
            reflectorDataVec.emplace_back(rflct);
            cameraData.reflectorVec.push_back(rflct);

            for (const wcoe::Reflector* rflct_ : reflectorVec)
                if (sq::bbox_in_frus(rflct_->bbox, rflct->frus))
                    reflectorDataVec.back().reflectorVec.push_back(rflct_);

            for (const wcoe::ModelSimple* model : modelSimpleVec)
                if (model->PROP_render && sq::bbox_in_frus(model->bbox, rflct->frus))
                    reflectorDataVec.back().modelSimpleVec.push_back(model);

            for (const wcoe::ModelSkelly* model : modelSkellyVec)
                if (sq::sphr_in_frus(model->sphere, rflct->frus))
                    reflectorDataVec.back().modelSkellyVec.push_back(model);

            for (const wcoe::SpotLight* light : spotLightVec)
                if (sq::frus_in_frus(light->frus, rflct->frus)) {
                    reflectorDataVec.back().spotLightVec.push_back(light);
                    if (light->PROP_shadow) shadowSpotLightSet.insert(light);
                }

            for (const wcoe::PointLight* light : pointLightVec)
                if (sq::sphr_in_frus(light->sphere, rflct->frus)) {
                    reflectorDataVec.back().pointLightVec.push_back(light);
                    if (light->PROP_shadow == true) {
                        shadowPointLightSet.insert(light);
                        for (uint ind = 0u; ind < 6u; ++ind)
                            if (!shadowPLFaceSetArr[ind].count(light) &&
                                sq::frus_in_frus(light->frusArr[ind], rflct->frus))
                                shadowPLFaceSetArr[ind].insert(light);
                    }
                }

            for (const wcoe::Decal* decal : decalVec) {
                // TODO: Add orientation check
                if (sq::bbox_in_frus(decal->bbox, rflct->frus) && decal->texDiff)
                    reflectorDataVec.back().decalVec.push_back(decal);
            }

            sort_container_by(reflectorDataVec.back().reflectorVec, [rflct]
                    (const wcoe::Reflector* a, const wcoe::Reflector* b) { return
                    maths::distance(rflct->frus.sphere.origin, a->bbox.sphere.origin) <
                    maths::distance(rflct->frus.sphere.origin, b->bbox.sphere.origin); });

            sort_container_by(reflectorDataVec.back().modelSimpleVec, [rflct]
                    (const wcoe::ModelSimple* a, const wcoe::ModelSimple* b) { return
                    maths::distance(rflct->frus.sphere.origin, a->bbox.sphere.origin) <
                    maths::distance(rflct->frus.sphere.origin, b->bbox.sphere.origin); });

            sort_container_by(reflectorDataVec.back().modelSkellyVec, [rflct]
                    (const wcoe::ModelSkelly* a, const wcoe::ModelSkelly* b) { return
                    maths::distance(rflct->frus.sphere.origin, a->sphere.origin) <
                    maths::distance(rflct->frus.sphere.origin, b->sphere.origin); });
        }
    }

    for (const wcoe::SpotLight* light : spotLightVec) {
        if (sq::frus_in_frus(light->frus, camera->frus)) {
            cameraData.spotLightVec.push_back(light);
            if (light->PROP_shadow) shadowSpotLightSet.insert(light);
        }

        if (shadowSpotLightSet.count(light)) {
            spotLightDataVec.emplace_back(light);

            for (const wcoe::Reflector* rflct : reflectorVec)
                if (maths::dot(light->PROP_position, rflct->normal) + rflct->offset < 0.f
                    && rflct->PROP_shadow && sq::bbox_in_frus(rflct->bbox, light->frus))
                    spotLightDataVec.back().reflectorVec.push_back(rflct);

            for (const wcoe::ModelSimple* model : modelSimpleVec)
                if (model->PROP_shadow && sq::bbox_in_frus(model->bbox, light->frus))
                    spotLightDataVec.back().modelSimpleVec.push_back(model);

            for (const wcoe::ModelSkelly* model : modelSkellyVec)
                if (model->PROP_shadow && sq::sphr_in_frus(model->sphere, light->frus))
                    spotLightDataVec.back().modelSkellyVec.push_back(model);

            sort_container_by(spotLightDataVec.back().reflectorVec, [light]
                    (const wcoe::Reflector* a, const wcoe::Reflector* b) { return
                    maths::distance(light->PROP_position, a->bbox.sphere.origin) <
                    maths::distance(light->PROP_position, b->bbox.sphere.origin); });

            sort_container_by(spotLightDataVec.back().modelSimpleVec, [light]
                    (const wcoe::ModelSimple* a, const wcoe::ModelSimple* b) { return
                    maths::distance(light->PROP_position, a->bbox.sphere.origin) <
                    maths::distance(light->PROP_position, b->bbox.sphere.origin); });

            sort_container_by(spotLightDataVec.back().modelSkellyVec, [light]
                    (const wcoe::ModelSkelly* a, const wcoe::ModelSkelly* b) { return
                    maths::distance(light->PROP_position, a->sphere.origin) <
                    maths::distance(light->PROP_position, b->sphere.origin); });
        }
    }

    for (const wcoe::PointLight* light : pointLightVec) {
        if (sq::sphr_in_frus(light->sphere, camera->frus)) {
            cameraData.pointLightVec.push_back(light);
            if (light->PROP_shadow == true) {
                shadowPointLightSet.insert(light);
                for (uint ind = 0u; ind < 6u; ++ind)
                    if (!shadowPLFaceSetArr[ind].count(light) &&
                        sq::frus_in_frus(light->frusArr[ind], camera->frus))
                        shadowPLFaceSetArr[ind].insert(light);
            }
        }

        if (shadowPointLightSet.count(light)) {
            pointLightDataVec.emplace_back(light);

            for (uint ind = 0u; ind < 6u; ++ind) {
                if (shadowPLFaceSetArr[ind].count(light)) {
                    pointLightDataVec.back().cullShadowFaceArr[ind] = false;

                    for (const wcoe::Reflector* rflct : reflectorVec)
                        if (maths::dot(light->PROP_position, rflct->normal) + rflct->offset < 0.f &&
                            rflct->PROP_shadow && sq::bbox_in_frus(rflct->bbox, light->frusArr[ind]))
                            pointLightDataVec.back().reflectorVecArr[ind].push_back(rflct);

                    for (const wcoe::ModelSimple* model : modelSimpleVec)
                        if (model->PROP_shadow && sq::bbox_in_frus(model->bbox, light->frusArr[ind]))
                            pointLightDataVec.back().modelSimpleVecArr[ind].push_back(model);

                    for (const wcoe::ModelSkelly* model : modelSkellyVec)
                        if (model->PROP_shadow && sq::sphr_in_frus(model->sphere, light->frusArr[ind]))
                            pointLightDataVec.back().modelSkellyVecArr[ind].push_back(model);

                    sort_container_by(pointLightDataVec.back().reflectorVecArr[ind], [light]
                            (const wcoe::Reflector* a, const wcoe::Reflector* b) { return
                            maths::distance(light->PROP_position, a->bbox.sphere.origin) <
                            maths::distance(light->PROP_position, b->bbox.sphere.origin); });

                    sort_container_by(pointLightDataVec.back().modelSimpleVecArr[ind], [light]
                            (const wcoe::ModelSimple* a, const wcoe::ModelSimple* b) { return
                            maths::distance(light->PROP_position, a->bbox.sphere.origin) <
                            maths::distance(light->PROP_position, b->bbox.sphere.origin); });

                    sort_container_by(pointLightDataVec.back().modelSkellyVecArr[ind], [light]
                            (const wcoe::ModelSkelly* a, const wcoe::ModelSkelly* b) { return
                            maths::distance(light->PROP_position, a->sphere.origin) <
                            maths::distance(light->PROP_position, b->sphere.origin); });
                }
            }
        }
    }

    for (const wcoe::ModelSimple* model : modelSimpleVec) {
        if (model->PROP_render && sq::bbox_in_frus(model->bbox, camera->frus))
            cameraData.modelSimpleVec.push_back(model);
        if (model->PROP_shadow == true) {
            for (uint ind = 0u; ind < 4u; ++ind)
                if (sq::bbox_in_orth(model->bbox, world.skylight->orthArrA[ind]))
                    skyLightData.modelSimpleVecArrA[ind].push_back(model);
            for (uint ind = 0u; ind < 2u; ++ind)
                if (sq::bbox_in_orth(model->bbox, world.skylight->orthArrB[ind]))
                    skyLightData.modelSimpleVecArrB[ind].push_back(model);
        }
    }

    for (const wcoe::ModelSkelly* model : modelSkellyVec) {
        if (sq::sphr_in_frus(model->sphere, camera->frus))
            cameraData.modelSkellyVec.push_back(model);
        for (uint ind = 0u; ind < 4u; ++ind)
            if (sq::sphr_in_orth(model->sphere, world.skylight->orthArrA[ind]))
                skyLightData.modelSkellyVecArrA[ind].push_back(model);
        for (uint ind = 0u; ind < 2u; ++ind)
            if (sq::sphr_in_orth(model->sphere, world.skylight->orthArrB[ind]))
                skyLightData.modelSkellyVecArrB[ind].push_back(model);
    }

    sort_container_by(cameraData.reflectorVec, [this]
            (const wcoe::Reflector* a, const wcoe::Reflector* b) { return
            maths::distance(camera->pos, a->bbox.sphere.origin) <
            maths::distance(camera->pos, b->bbox.sphere.origin); });

    sort_container_by(cameraData.modelSimpleVec, [this]
            (const wcoe::ModelSimple* a, const wcoe::ModelSimple* b) { return
            maths::distance(camera->pos, a->bbox.sphere.origin) <
            maths::distance(camera->pos, b->bbox.sphere.origin); });

    sort_container_by(cameraData.modelSkellyVec, [this]
            (const wcoe::ModelSkelly* a, const wcoe::ModelSkelly* b) { return
            maths::distance(camera->pos, a->sphere.origin) <
            maths::distance(camera->pos, b->sphere.origin); });

    for (uint ind = 0u; ind < 4u; ++ind) {
        sort_container_by(skyLightData.reflectorVecArrA[ind], [this]
            (const wcoe::Reflector* a, const wcoe::Reflector* b) { return
            maths::dot(world.skylight->PROP_direction, a->bbox.sphere.origin) <
            maths::dot(world.skylight->PROP_direction, b->bbox.sphere.origin); });

        sort_container_by(skyLightData.modelSimpleVecArrA[ind], [this]
            (const wcoe::ModelSimple* a, const wcoe::ModelSimple* b) { return
            maths::dot(world.skylight->PROP_direction, a->bbox.sphere.origin) <
            maths::dot(world.skylight->PROP_direction, b->bbox.sphere.origin); });

        sort_container_by(skyLightData.modelSkellyVecArrA[ind], [this]
            (const wcoe::ModelSkelly* a, const wcoe::ModelSkelly* b) { return
            maths::dot(world.skylight->PROP_direction, a->sphere.origin) <
            maths::dot(world.skylight->PROP_direction, b->sphere.origin); });
    }

    for (uint ind = 0u; ind < 2u; ++ind) {
        sort_container_by(skyLightData.reflectorVecArrB[ind], [this]
            (const wcoe::Reflector* a, const wcoe::Reflector* b) { return
            maths::dot(world.skylight->PROP_direction, a->bbox.sphere.origin) <
            maths::dot(world.skylight->PROP_direction, b->bbox.sphere.origin); });

        sort_container_by(skyLightData.modelSimpleVecArrB[ind], [this]
            (const wcoe::ModelSimple* a, const wcoe::ModelSimple* b) { return
            maths::dot(world.skylight->PROP_direction, a->bbox.sphere.origin) <
            maths::dot(world.skylight->PROP_direction, b->bbox.sphere.origin); });

        sort_container_by(skyLightData.modelSkellyVecArrB[ind], [this]
            (const wcoe::ModelSkelly* a, const wcoe::ModelSkelly* b) { return
            maths::dot(world.skylight->PROP_direction, a->sphere.origin) <
            maths::dot(world.skylight->PROP_direction, b->sphere.origin); });
    }

    for (const wcoe::Decal* decal : decalVec) {
        // TODO: Add orientation check
        if (sq::bbox_in_frus(decal->bbox, camera->frus))
            cameraData.decalVec.push_back(decal);
    }
}


void Renderer::update_settings() {    
    //string defLightBase = "";
    //if (INFO.shadlarge) defLightBase += "#define LARGE\n";
    //if (INFO.shadfilter) defLightBase += "#define FILTER";
    //string defLightShad = defLightBase + "\n#define SHADOW";

    /// Lighting
    //preprocs.load(FS_part_soft_skylight, "particles/soft/skylight_fs", defLightBase);
    //preprocs.load(FS_part_soft_spot_none, "particles/soft/spotlight_fs", defLightBase);
    //preprocs.load(FS_part_soft_spot_shad, "particles/soft/spotlight_fs", defLightShad);
    //preprocs.load(FS_part_soft_point_none, "particles/soft/pointlight_fs", defLightBase);
    //preprocs.load(FS_part_soft_point_shad, "particles/soft/pointlight_fs", defLightShad);

    shadows->update_settings();
    gbuffers->update_settings();
    lighting->update_settings();
    pretties->update_settings();
    reflects->update_settings();
}
