#include <algorithm>
#include <map>
#include <set>

#include <sqee/assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/app/MessageBus.hpp>

#include <sqee/misc/StringCast.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/PreProcessor.hpp>
#include <sqee/app/Settings.hpp>
#include <sqee/app/Resources.hpp>
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

    void update_render_lists(const sq::Entity* _e);
    void update_reflect_lists(const sq::Entity* _e);
    void update_light_lists(const sq::Entity* _e);

    void sort_render_lists();

    Renderer& renderer;

    std::set<const SpotLightComponent*> visibleSpotLightSet;
    std::map<const PointLightComponent*, array<bool, 6>> visiblePointLightMap;
};


void Renderer::Impl::update_render_lists(const sq::Entity* _e) {
    auto model = _e->get_component<ModelComponent>();
    auto decal = _e->get_component<DecalComponent>();
    auto spotLight = _e->get_component<SpotLightComponent>();
    auto pointLight = _e->get_component<PointLightComponent>();
    auto reflect = _e->get_component<ReflectComponent>();

    if (model != nullptr && reflect == nullptr && model->PROP_render == true)
        if (sq::bbox_in_frus(model->bbox, renderer.camera.frus)) {
            if (model->arma) renderer.cameraData.modelSkellyVec.push_back(model);
            else renderer.cameraData.modelSimpleVec.push_back(model); }

    if (model != nullptr && reflect != nullptr && model->PROP_render == true) {
        float camOffset = maths::dot(renderer.camera.pos, reflect->normal);
        float offset = maths::dot(-reflect->normal, reflect->trans);
        if (camOffset + offset > 0.f && sq::bbox_in_frus(model->bbox, renderer.camera.frus)) {
            renderer.reflectDataVec.emplace_back(reflect, model, sq::reflect_Frustum(renderer.camera.frus, reflect->normal, reflect->trans));
            renderer.cameraData.modelSimpleVec.push_back(model); } }

    if (model != nullptr && model->PROP_shadow == true) {
        for (uint csm = 0u; csm < 4u; ++csm)
            if (sq::bbox_in_orth(model->bbox, renderer.world.skylight->orthArrA[csm])) {
                if (model->arma && model->skin->hasPunchThru) renderer.skyLightData.modelSkellyPunchVecArrA[csm].push_back(model);
                else if (model->skin->hasPunchThru) renderer.skyLightData.modelSimplePunchVecArrA[csm].push_back(model);
                else if (model->arma) renderer.skyLightData.modelSkellyVecArrA[csm].push_back(model);
                else renderer.skyLightData.modelSimpleVecArrA[csm].push_back(model); }

        for (uint csm = 0u; csm < 2u; ++csm)
            if (sq::bbox_in_orth(model->bbox, renderer.world.skylight->orthArrB[csm])) {
                if (model->arma && model->skin->hasPunchThru) renderer.skyLightData.modelSkellyPunchVecArrB[csm].push_back(model);
                else if (model->skin->hasPunchThru) renderer.skyLightData.modelSimplePunchVecArrB[csm].push_back(model);
                else if (model->arma) renderer.skyLightData.modelSkellyVecArrB[csm].push_back(model);
                else renderer.skyLightData.modelSimpleVecArrB[csm].push_back(model); }
    }

    if (decal != nullptr && decal->PROP_alpha > 0.001f)
        if (sq::bbox_in_frus(decal->bbox, renderer.camera.frus)) {
            if (decal->texDiff && decal->texNorm && decal->texSpec)
                renderer.cameraData.decalCompleteVec.push_back(decal);
            else renderer.cameraData.decalPartialVec.push_back(decal); }

    if (spotLight != nullptr && spotLight->PROP_texsize != 0u)
        if (sq::frus_in_frus(spotLight->frus, renderer.camera.frus))
            renderer.cameraData.spotLightShadowVec.push_back(spotLight),
            visibleSpotLightSet.emplace(spotLight);

    if (spotLight != nullptr && spotLight->PROP_texsize == 0u)
        if (sq::frus_in_frus(spotLight->frus, renderer.camera.frus))
            renderer.cameraData.spotLightNoShadowVec.push_back(spotLight);

    if (pointLight != nullptr && pointLight->PROP_texsize != 0u)
        if (sq::sphr_in_frus(pointLight->sphere, renderer.camera.frus)) {
            auto& visible = visiblePointLightMap[pointLight];
            visible[0] = sq::frus_in_frus(pointLight->frusArr[0], renderer.camera.frus);
            visible[1] = sq::frus_in_frus(pointLight->frusArr[1], renderer.camera.frus);
            visible[2] = sq::frus_in_frus(pointLight->frusArr[2], renderer.camera.frus);
            visible[3] = sq::frus_in_frus(pointLight->frusArr[3], renderer.camera.frus);
            visible[4] = sq::frus_in_frus(pointLight->frusArr[4], renderer.camera.frus);
            visible[5] = sq::frus_in_frus(pointLight->frusArr[5], renderer.camera.frus);
            renderer.cameraData.pointLightShadowVec.push_back(pointLight); }

    if (pointLight != nullptr && pointLight->PROP_texsize == 0u)
        if (sq::sphr_in_frus(pointLight->sphere, renderer.camera.frus))
            renderer.cameraData.pointLightNoShadowVec.push_back(pointLight);

    for (auto& child : _e->get_children())
        update_render_lists(child.get());
}


void Renderer::Impl::update_reflect_lists(const sq::Entity* _e) {
    auto model = _e->get_component<ModelComponent>();
    auto decal = _e->get_component<DecalComponent>();
    auto spotLight = _e->get_component<SpotLightComponent>();
    auto pointLight = _e->get_component<PointLightComponent>();

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


void Renderer::Impl::update_light_lists(const sq::Entity* _e) {
    auto model = _e->get_component<ModelComponent>();

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
        return maths::distance(renderer.camera.pos, a->bbox.origin) <
               maths::distance(renderer.camera.pos, b->bbox.origin); };

    sort_container_by(renderer.cameraData.modelSimpleVec, cameraCompareFunc);
    sort_container_by(renderer.cameraData.modelSkellyVec, cameraCompareFunc);

    auto skyLightCompareFunc = [this](const ModelComponent* a, const ModelComponent* b) {
        return maths::dot(renderer.world.skylight->PROP_direction, a->bbox.origin) <
               maths::dot(renderer.world.skylight->PROP_direction, b->bbox.origin); };

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
            return maths::distance(renderer.camera.pos, maths::reflect(a->bbox.origin, data.reflect->normal, data.reflect->trans)) <
                   maths::distance(renderer.camera.pos, maths::reflect(a->bbox.origin, data.reflect->normal, data.reflect->trans)); };

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
    impl->update_render_lists(&world.root);
    impl->update_reflect_lists(&world.root);
    for (auto light : impl->visibleSpotLightSet)
        spotLightDataVec.emplace_back(light);
    for (const auto& light : impl->visiblePointLightMap)
        pointLightDataVec.emplace_back(light.first, light.second);
    impl->update_light_lists(&world.root);
    impl->sort_render_lists();

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


Renderer::Renderer(sq::MessageBus& _messageBus, const sq::Settings& _settings,
                   const sq::PreProcessor& _preprocs, const sq::Pipeline& _pipeline,
                   const sq::Camera& _camera, const World& _world)
    : messageBus(_messageBus), settings(_settings), preprocs(_preprocs),
      pipeline(_pipeline), camera(_camera), world(_world), impl(new Impl(*this)) {

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

    Vec2U fullSize = Vec2U(settings.get<int>("app_width"),
                           settings.get<int>("app_height"));
    Vec2U halfSize = fullSize / 2u;

    preprocs.load(VS_fullscreen, "generic/fullscreen_vs");
    preprocs.load(VS_stencil_base, "generic/stencil_base_vs");
    preprocs.load(VS_stencil_refl, "generic/stencil_refl_vs");
    preprocs.load(FS_passthrough, "generic/passthrough_fs");

    Vec2F hPixSize = Vec2F(1.f, 1.f) / Vec2F(halfSize);
    string defHPixSize = "#define PIXSIZE " + sq::glsl_string(hPixSize);
    preprocs.load(FS_fill_space, "generic/fill_space_fs", defHPixSize);

    preprocs.load(VS_part_soft_vertex, "particles/soft/vertex_vs");
    preprocs.load(GS_part_soft_geometry, "particles/soft/geometry_gs");
    preprocs.load(FS_part_soft_ambient, "particles/soft/ambient_fs");
    preprocs.load(FS_part_soft_write, "particles/soft/write_fs");
}


void Renderer::configure() {
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



void Renderer::draw_debug_bounds() {
    static sq::VertexArray vertArr;
    static sq::FixedBuffer vertBuf(gl::ARRAY_BUFFER);
    static sq::FixedBuffer elemBufBox(gl::ELEMENT_ARRAY_BUFFER);
    static sq::FixedBuffer elemBufFrus(gl::ELEMENT_ARRAY_BUFFER);

    static bool first = true;
    if (first) { first = false;
        vertArr.add_attribute(vertBuf, 0u, 0u, 12u, 3u, gl::FLOAT, false);
        const uchar linesBox[24] { 0,1, 1,3, 3,2, 2,0, 4,5, 5,7, 7,6, 6,4, 0,4, 1,5, 3,7, 2,6 };
        const uchar linesFrus[16] { 0,1, 0,2, 0,3, 0,4, 1,2, 1,3, 2,4, 3,4 };
        elemBufBox.allocate_constant(24u, linesBox);
        elemBufFrus.allocate_constant(24u, linesFrus);
        vertBuf.allocate_editable(96u, nullptr);
    }

    pipeline.use_shader(VS_stencil_base);
    pipeline.disable_stages(0, 1, 1);
    vertArr.bind();

    //gl::ClearColor(1, 1, 1, 0);
    //sq::CLEAR_COLOR_DEPTH_STENC();

    sq::DEPTH_OFF();
    sq::STENCIL_OFF();

    gl::LineWidth(2.f);
    VS_stencil_base.set_mat<Mat4F>("matrix", Mat4F());

    vertBuf.bind(); // should not be neeeded, possible bug in nvidia driver?

    vertArr.set_element_buffer(elemBufBox);

//    for (const ModelComponent* mc : cameraData.XmodelSimpleVec) {
//        const Vec3F offs[3] {
//            Vec3F{mc->bbox.normX * mc->bbox.sizeX},
//            Vec3F{mc->bbox.normY * mc->bbox.sizeY},
//            Vec3F{mc->bbox.normZ * mc->bbox.sizeZ} };

//        const Vec3F points[8] {
//            Vec3F{mc->bbox.origin -offs[0] -offs[1] -offs[2]},
//            Vec3F{mc->bbox.origin -offs[0] -offs[1] +offs[2]},
//            Vec3F{mc->bbox.origin -offs[0] +offs[1] -offs[2]},
//            Vec3F{mc->bbox.origin -offs[0] +offs[1] +offs[2]},
//            Vec3F{mc->bbox.origin +offs[0] -offs[1] -offs[2]},
//            Vec3F{mc->bbox.origin +offs[0] -offs[1] +offs[2]},
//            Vec3F{mc->bbox.origin +offs[0] +offs[1] -offs[2]},
//            Vec3F{mc->bbox.origin +offs[0] +offs[1] +offs[2]} };

//        vertBuf.update(0u, 96u, points);

//        gl::DrawElements(gl::LINES, 24, gl::UNSIGNED_BYTE, nullptr);
//    }

//    vertArr.set_element_buffer(elemBufFrus);

//    for (const SpotLightComponent* lc : cameraData.XspotLightShadowVec) {
//        vertBuf.update(0u, 96u, &lc->frus.points);

//        gl::DrawElements(gl::LINES, 16, gl::UNSIGNED_BYTE, nullptr);
//    }

//    for (const PointLightComponent* lc : cameraData.XpointLightShadowVec) {
//        for (uint ind = 0u; ind < 6; ++ind) {
//            vertBuf.update(0u, 96u, &lc->frusArr[ind].points);
//            gl::DrawElements(gl::LINES, 16, gl::UNSIGNED_BYTE, nullptr);
//        }
//    }


//    gl::BindBuffer(gl::ARRAY_BUFFER, 0u);
}
