#include <algorithm>
#include <map>
#include <set>

#include <sqee/assert.hpp>
#include <sqee/debug/Logging.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/PreProcessor.hpp>
#include <sqee/gl/Shaders.hpp>
#include <sqee/render/Armature.hpp>
#include <sqee/render/Skin.hpp>
#include <sqee/maths/Volumes.hpp>
#include <sqee/misc/StringCast.hpp>

#include "../wcoe/Camera.hpp"
#include "../wcoe/Ambient.hpp"
#include "../wcoe/SkyBox.hpp"
#include "../wcoe/SkyLight.hpp"
#include "../wcoe/World.hpp"

#include "../components/Animator.hpp"
#include "../components/Transform.hpp"
#include "../components/Model.hpp"
#include "../components/Decal.hpp"
#include "../components/SpotLight.hpp"
#include "../components/PointLight.hpp"
#include "../components/Reflect.hpp"

#include "Shadows.hpp"
#include "Gbuffers.hpp"
#include "Lighting.hpp"
#include "Pretties.hpp"
#include "Reflects.hpp"
#include "Renderer.hpp"

using namespace sqt;
namespace maths = sq::maths;

Renderer::~Renderer() = default;

struct Renderer::Impl {
    Impl(Renderer& _renderer) : renderer(_renderer) {}

    void update_render_lists(const EntityRPG* _e);
    void update_reflect_lists(const EntityRPG* _e);
    void update_light_lists(const EntityRPG* _e);

    void sort_render_lists();

    Renderer& renderer;

    std::set<const SpotLightComponent*> visibleSpotLightSet;
    std::map<const PointLightComponent*, array<bool, 6>> visiblePointLightMap;
};


void Renderer::Impl::update_render_lists(const EntityRPG* _e) {
    const auto* model      = _e->try_get_component<ModelComponent>();
    const auto* decal      = _e->try_get_component<DecalComponent>();
    const auto* spotLight  = _e->try_get_component<SpotLightComponent>();
    const auto* pointLight = _e->try_get_component<PointLightComponent>();
    const auto* reflect    = _e->try_get_component<ReflectComponent>();

    const Camera& camera = renderer.world.get_Camera();

    if (model != nullptr && reflect == nullptr && model->PROP_render == true)
        if (sq::bbox_in_frus(model->bbox, camera.frus)) {
            if (model->arma) renderer.cameraData.modelSkellyVec.push_back(model);
            else renderer.cameraData.modelSimpleVec.push_back(model); }

    if (model != nullptr && reflect != nullptr && model->PROP_render == true) {
        float camOffset = maths::dot(camera.PROP_position, reflect->normal);
        if (camOffset + maths::dot(-reflect->normal, reflect->trans) > 0.f && sq::bbox_in_frus(model->bbox, camera.frus)) {
            renderer.reflectDataVec.emplace_back(reflect, model, sq::reflect_Frustum(camera.frus, reflect->normal, reflect->trans));
            renderer.cameraData.modelSimpleVec.push_back(model); } }

    if (renderer.world.check_SkyLight() == true) {
        if (model != nullptr && model->PROP_shadow == true) {
            for (uint csm = 0u; csm < 4u; ++csm)
                if (sq::bbox_in_orth(model->bbox, renderer.world.get_SkyLight().orthArrA[csm])) {
                    if (model->arma && model->skin->hasPunchThru) renderer.skyLightData.modelSkellyPunchVecArrA[csm].push_back(model);
                    else if (model->skin->hasPunchThru) renderer.skyLightData.modelSimplePunchVecArrA[csm].push_back(model);
                    else if (model->arma) renderer.skyLightData.modelSkellyVecArrA[csm].push_back(model);
                    else renderer.skyLightData.modelSimpleVecArrA[csm].push_back(model); }

            for (uint csm = 0u; csm < 2u; ++csm)
                if (sq::bbox_in_orth(model->bbox, renderer.world.get_SkyLight().orthArrB[csm])) {
                    if (model->arma && model->skin->hasPunchThru) renderer.skyLightData.modelSkellyPunchVecArrB[csm].push_back(model);
                    else if (model->skin->hasPunchThru) renderer.skyLightData.modelSimplePunchVecArrB[csm].push_back(model);
                    else if (model->arma) renderer.skyLightData.modelSkellyVecArrB[csm].push_back(model);
                    else renderer.skyLightData.modelSimpleVecArrB[csm].push_back(model); }
        }
    }

    if (decal != nullptr && decal->PROP_alpha > 0.001f)
        if (sq::bbox_in_frus(decal->bbox, camera.frus)) {
            if (decal->texDiff && decal->texNorm && decal->texSpec)
                renderer.cameraData.decalCompleteVec.push_back(decal);
            else renderer.cameraData.decalPartialVec.push_back(decal); }

    if (spotLight != nullptr && spotLight->PROP_texsize != 0u)
        if (sq::frus_in_frus(spotLight->frus, camera.frus))
            renderer.cameraData.spotLightShadowVec.push_back(spotLight),
            visibleSpotLightSet.emplace(spotLight);

    if (spotLight != nullptr && spotLight->PROP_texsize == 0u)
        if (sq::frus_in_frus(spotLight->frus, camera.frus))
            renderer.cameraData.spotLightNoShadowVec.push_back(spotLight);

    if (pointLight != nullptr && pointLight->PROP_texsize != 0u)
        if (sq::sphr_in_frus(pointLight->sphere, camera.frus)) {
            auto& visible = visiblePointLightMap[pointLight];
            visible[0] = sq::frus_in_frus(pointLight->frusArr[0], camera.frus);
            visible[1] = sq::frus_in_frus(pointLight->frusArr[1], camera.frus);
            visible[2] = sq::frus_in_frus(pointLight->frusArr[2], camera.frus);
            visible[3] = sq::frus_in_frus(pointLight->frusArr[3], camera.frus);
            visible[4] = sq::frus_in_frus(pointLight->frusArr[4], camera.frus);
            visible[5] = sq::frus_in_frus(pointLight->frusArr[5], camera.frus);
            renderer.cameraData.pointLightShadowVec.push_back(pointLight); }

    if (pointLight != nullptr && pointLight->PROP_texsize == 0u)
        if (sq::sphr_in_frus(pointLight->sphere, camera.frus))
            renderer.cameraData.pointLightNoShadowVec.push_back(pointLight);

    for (auto& child : _e->get_children())
        update_render_lists(child.get());
}


void Renderer::Impl::update_reflect_lists(const EntityRPG* _e) {
    auto model      = _e->try_get_component<ModelComponent>();
    auto decal      = _e->try_get_component<DecalComponent>();
    auto spotLight  = _e->try_get_component<SpotLightComponent>();
    auto pointLight = _e->try_get_component<PointLightComponent>();

    if (model != nullptr && model->PROP_render == true)
        for (auto& data : renderer.reflectDataVec)
            if (sq::bbox_in_frus(model->bbox, data.frus)) {
                if (model->arma) data.modelSkellyVec.push_back(model);
                else data.modelSimpleVec.push_back(model); }

    if (decal != nullptr && decal->PROP_alpha > 0.001f)
        for (auto& data : renderer.reflectDataVec)
            if (sq::bbox_in_frus(decal->bbox, data.frus))
                if (decal->texDiff) data.decalDiffVec.push_back(decal);

    if (spotLight != nullptr && spotLight->PROP_texsize != 0u)
        for (auto& data : renderer.reflectDataVec)
            if (sq::frus_in_frus(spotLight->frus, data.frus))
                data.spotLightShadowVec.push_back(spotLight),
                visibleSpotLightSet.emplace(spotLight);

    if (spotLight != nullptr && spotLight->PROP_texsize == 0u)
        for (auto& data : renderer.reflectDataVec)
        if (sq::frus_in_frus(spotLight->frus, data.frus))
            data.spotLightNoShadowVec.push_back(spotLight);

    if (pointLight != nullptr && pointLight->PROP_texsize != 0u)
        for (auto& data : renderer.reflectDataVec)
            if (sq::sphr_in_frus(pointLight->sphere, data.frus)) {
                auto& visible = visiblePointLightMap[pointLight];
                visible[0] |= sq::frus_in_frus(pointLight->frusArr[0], data.frus);
                visible[1] |= sq::frus_in_frus(pointLight->frusArr[1], data.frus);
                visible[2] |= sq::frus_in_frus(pointLight->frusArr[2], data.frus);
                visible[3] |= sq::frus_in_frus(pointLight->frusArr[3], data.frus);
                visible[4] |= sq::frus_in_frus(pointLight->frusArr[4], data.frus);
                visible[5] |= sq::frus_in_frus(pointLight->frusArr[5], data.frus);
                data.pointLightShadowVec.push_back(pointLight); }

    if (pointLight != nullptr && pointLight->PROP_texsize == 0u)
        for (auto& data : renderer.reflectDataVec)
            if (sq::sphr_in_frus(pointLight->sphere, data.frus))
                data.pointLightNoShadowVec.push_back(pointLight);

    for (auto& child : _e->get_children())
        update_reflect_lists(child.get());
}


void Renderer::Impl::update_light_lists(const EntityRPG* _e) {
    auto model = _e->try_get_component<ModelComponent>();

    if (model != nullptr && model->PROP_shadow == true)
        for (auto& data : renderer.spotLightDataVec)
            if (sq::bbox_in_frus(model->bbox, data.light->frus)) {
                if (model->arma && model->skin->hasPunchThru) data.modelSkellyPunchVec.push_back(model);
                else if (model->skin->hasPunchThru) data.modelSimplePunchVec.push_back(model);
                else if (model->arma) data.modelSkellyVec.push_back(model);
                else data.modelSimpleVec.push_back(model); }

    if (model != nullptr && model->PROP_shadow == true)
        for (auto& data : renderer.pointLightDataVec)
            for (uint face = 0u; face < 6u; ++face)
                if (data.visibleFaceArr[face] && sq::bbox_in_frus(model->bbox, data.light->frusArr[face])) {
                    if (model->arma && model->skin->hasPunchThru) data.modelSkellyPunchVecArr[face].push_back(model);
                    else if (model->skin->hasPunchThru) data.modelSimplePunchVecArr[face].push_back(model);
                    else if (model->arma) data.modelSkellyVecArr[face].push_back(model);
                    else data.modelSimpleVecArr[face].push_back(model); }

    for (auto& child : _e->get_children())
        update_light_lists(child.get());
}


void Renderer::Impl::sort_render_lists() {
    auto sort_container_by = [](auto& _container, const auto _compare) {
        std::sort(_container.begin(), _container.end(), _compare); };

    auto cameraCompareFunc = [this](const ModelComponent* a, const ModelComponent* b) {
        return maths::distance(renderer.world.get_Camera().PROP_position, a->bbox.origin) <
               maths::distance(renderer.world.get_Camera().PROP_position, b->bbox.origin); };

    sort_container_by(renderer.cameraData.modelSimpleVec, cameraCompareFunc);
    sort_container_by(renderer.cameraData.modelSkellyVec, cameraCompareFunc);

    auto skyLightCompareFunc = [this](const ModelComponent* a, const ModelComponent* b) {
        return maths::dot(renderer.world.get_SkyLight().PROP_direction, a->bbox.origin) <
               maths::dot(renderer.world.get_SkyLight().PROP_direction, b->bbox.origin); };

    for (uint csm = 0u; csm < 4u; ++csm) {
        sort_container_by(renderer.skyLightData.modelSimpleVecArrA[csm], skyLightCompareFunc);
        sort_container_by(renderer.skyLightData.modelSimplePunchVecArrA[csm], skyLightCompareFunc);
        sort_container_by(renderer.skyLightData.modelSkellyVecArrA[csm], skyLightCompareFunc);
        sort_container_by(renderer.skyLightData.modelSkellyPunchVecArrA[csm], skyLightCompareFunc);
    }

    for (uint csm = 0u; csm < 2u; ++csm) {
        sort_container_by(renderer.skyLightData.modelSimpleVecArrB[csm], skyLightCompareFunc);
        sort_container_by(renderer.skyLightData.modelSimplePunchVecArrB[csm], skyLightCompareFunc);
        sort_container_by(renderer.skyLightData.modelSkellyVecArrB[csm], skyLightCompareFunc);
        sort_container_by(renderer.skyLightData.modelSkellyPunchVecArrB[csm], skyLightCompareFunc);
    }

    for (auto& data : renderer.spotLightDataVec) {
        auto spotLightCompareFunc = [&](const ModelComponent* a, const ModelComponent* b) {
            return maths::distance(data.light->frus.points[0], a->bbox.origin) <
                   maths::distance(data.light->frus.points[0], b->bbox.origin); };

        sort_container_by(data.modelSimpleVec, spotLightCompareFunc);
        sort_container_by(data.modelSimplePunchVec, spotLightCompareFunc);
        sort_container_by(data.modelSkellyVec, spotLightCompareFunc);
        sort_container_by(data.modelSkellyPunchVec, spotLightCompareFunc);
    }

    for (auto& data : renderer.pointLightDataVec) {
        for (uint face = 0u; face < 6u; ++face) {
            auto pointLightCompareFunc = [&](const ModelComponent* a, const ModelComponent* b) {
                return maths::distance(data.light->sphere.origin, a->bbox.origin) <
                       maths::distance(data.light->sphere.origin, b->bbox.origin); };

            sort_container_by(data.modelSimpleVecArr[face], pointLightCompareFunc);
            sort_container_by(data.modelSimplePunchVecArr[face], pointLightCompareFunc);
            sort_container_by(data.modelSkellyVecArr[face], pointLightCompareFunc);
            sort_container_by(data.modelSkellyPunchVecArr[face], pointLightCompareFunc);
        }
    }

    for (auto& data : renderer.reflectDataVec) {
        auto reflectCompareFunc = [&](const ModelComponent* a, const ModelComponent* b) {
            return maths::distance(renderer.world.get_Camera().PROP_position, maths::reflect(
                                       a->bbox.origin, data.reflect->normal, data.reflect->trans)) <
                   maths::distance(renderer.world.get_Camera().PROP_position, maths::reflect(
                                       b->bbox.origin, data.reflect->normal, data.reflect->trans)); };

        sort_container_by(data.modelSimpleVec, reflectCompareFunc);
        sort_container_by(data.modelSkellyVec, reflectCompareFunc);
    }
}


void Renderer::render_scene() {
    // clear old render lists
    cameraData = CameraData();
    skyLightData = SkyLightData();
    impl->visibleSpotLightSet.clear();
    impl->visiblePointLightMap.clear();
    spotLightDataVec.clear();
    pointLightDataVec.clear();
    reflectDataVec.clear();

    // fill and sort new render lists
    impl->update_render_lists(&world.get_RootEntity());
    impl->update_reflect_lists(&world.get_RootEntity());
    for (const auto* light : impl->visibleSpotLightSet)
        spotLightDataVec.emplace_back(light);
    for (const auto& light : impl->visiblePointLightMap)
        pointLightDataVec.emplace_back(light.first, light.second);
    impl->update_light_lists(&world.get_RootEntity());
    impl->sort_render_lists();

    // setup some state
    gl::DepthFunc(gl::LEQUAL);
    world.get_Camera().ubo.bind(0u);
    pipeline.bind();

    // render to shadow maps
    shadows->setup_render_state();
    shadows->render_shadows_sky();
    shadows->render_shadows_spot();
    shadows->render_shadows_point();

    // write gbuffers for main view
    gbuffers->render_gbuffers_base();

    // render ssao texture for main view
    pretties->render_post_gbuffers();

    // render lighting for main view
    lighting->render_lighting_base();

    // render reflections to main view
    reflects->render_reflections();

    //renderer->render_particles();

    // render light shafts and bloom
    pretties->render_post_lighting();

    // composite final image to screen
    pretties->render_final_screen();
}


Renderer::Renderer(const RpgOptions& _options, const World& _world)
    : options(_options), world(_world), impl(new Impl(*this)) {

    // Import GLSL Headers
    preprocs.import_header("headers/blocks/msimple");
    preprocs.import_header("headers/blocks/mskelly");
    preprocs.import_header("headers/blocks/ambient");
    preprocs.import_header("headers/blocks/skylight");
    preprocs.import_header("headers/blocks/spotlight");
    preprocs.import_header("headers/blocks/pointlight");
    preprocs.import_header("headers/blocks/reflect");
    preprocs.import_header("headers/blocks/skybox");
    preprocs.import_header("headers/blocks/decal");
    preprocs.import_header("headers/shadow/sample_sky");
    preprocs.import_header("headers/shadow/sample_spot");
    preprocs.import_header("headers/shadow/sample_point");

    shadows.reset(new Shadows(*this));
    gbuffers.reset(new Gbuffers(*this));
    lighting.reset(new Lighting(*this));
    pretties.reset(new Pretties(*this));
    reflects.reset(new Reflects(*this));

    /*// Particles /////
    gl::GenVertexArrays(1, &partVAO);
    gl::GenBuffers(1, &partVBO);
    gl::GenBuffers(1, &partIBO);

    gl::BindVertexArray(partVAO);
    gl::BindBuffer(gl::ARRAY_BUFFER, partVBO);
    gl::VertexAttribPointer(0, 4, gl::FLOAT, false, 32, (void*)(0));
    gl::VertexAttribPointer(1, 4, gl::FLOAT, false, 32, (void*)(16));
    gl::EnableVertexAttribArray(0); gl::EnableVertexAttribArray(1);*/

    VS_stencil_base.add_uniform("matrix"); // mat4
    VS_stencil_refl.add_uniform("matrix"); // mat4

    Vec2U fullSize = options.WindowSize;
    Vec2U halfSize = fullSize / 2u;

    preprocs.load(VS_fullscreen, "generic/fullscreen_vs");
    preprocs.load(VS_stencil_base, "generic/stencil_base_vs");
    preprocs.load(VS_stencil_refl, "generic/stencil_refl_vs");
    preprocs.load(FS_passthrough, "generic/passthrough_fs");

    Vec2F hPixSize = Vec2F(1.f, 1.f) / Vec2F(halfSize);
    string defHPixSize = "#define PIXSIZE " + sq::glsl_string(hPixSize);
    preprocs.load(FS_fill_space, "generic/fill_space_fs", defHPixSize);

    /*preprocs.load(VS_part_soft_vertex, "particles/soft/vertex_vs");
    preprocs.load(GS_part_soft_geometry, "particles/soft/geometry_gs");
    preprocs.load(FS_part_soft_ambient, "particles/soft/ambient_fs");
    preprocs.load(FS_part_soft_write, "particles/soft/write_fs");*/
}


void Renderer::update_options() {
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

    shadows->update_options();
    gbuffers->update_options();
    lighting->update_options();
    pretties->update_options();
    reflects->update_options();
}
