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

#include "../components/Transform.hpp"
#include "../components/Model.hpp"
#include "../components/SpotLight.hpp"
#include "../components/PointLight.hpp"
#include "../components/Reflect.hpp"
#include "../components/Decal.hpp"

#include "Shadows.hpp"
#include "Gbuffers.hpp"
#include "Lighting.hpp"
#include "Pretties.hpp"
#include "Reflects.hpp"
#include "Renderer.hpp"

using namespace sqt::rndr;
namespace maths = sq::maths;

Renderer::~Renderer() = default;

template<class Container, class Compare> inline void
sort_container_by(Container& _container, const Compare _compare) {
    std::sort(_container.begin(), _container.end(), _compare); }

struct Renderer::Implementation {
    Implementation(Renderer& _renderer) : renderer(_renderer) {}

    void configure_and_update_entity(sq::Entity* _e);

    void configure_component_Transform(TransformComponent* _c);
    void refresh_component_Transform(TransformComponent* _c);
    void update_component_Transform(TransformComponent* _c);

    void configure_component_Model(ModelComponent* _c);
    void refresh_component_Model(ModelComponent* _c);
    void update_component_Model(ModelComponent* _c);

    void configure_component_SpotLight(SpotLightComponent* _c);
    void refresh_component_SpotLight(SpotLightComponent* _c);
    void update_component_SpotLight(SpotLightComponent* _c);

    void configure_component_PointLight(PointLightComponent* _c);
    void refresh_component_PointLight(PointLightComponent* _c);
    void update_component_PointLight(PointLightComponent* _c);

    void configure_component_Reflect(ReflectComponent* _c);
    void refresh_component_Reflect(ReflectComponent* _c);
    void update_component_Reflect(ReflectComponent* _c);

    void configure_component_Decal(DecalComponent* _c);
    void refresh_component_Decal(DecalComponent* _c);
    void update_component_Decal(DecalComponent* _c);

    void update_render_lists(const sq::Entity* _e);
    void update_reflect_lists(const sq::Entity* _e);
    void update_light_lists(const sq::Entity* _e);

    void sort_render_lists();

    Renderer& renderer;

    std::set<const SpotLightComponent*> visibleSpotLightSet;
    std::map<const PointLightComponent*, array<bool, 6>> visiblePointLightMap;
};


void Renderer::Implementation::configure_and_update_entity(sq::Entity* _e) {
    if (auto c = _e->get_component<TransformComponent>()) {
        if (c->needsConfigure == true) configure_component_Transform(c), c->needsConfigure = true;
        if (c->needsRefresh == true) refresh_component_Transform(c), c->needsRefresh = true;
    }

    if (auto c = _e->get_component<ModelComponent>()) {
        if (c->needsConfigure == true) configure_component_Model(c), c->needsConfigure = true;
        if (c->needsRefresh == true) refresh_component_Model(c), c->needsRefresh = true; }

    if (auto c = _e->get_component<SpotLightComponent>()) {
        if (c->needsConfigure == true) configure_component_SpotLight(c), c->needsConfigure = true;
        if (c->needsRefresh == true) refresh_component_SpotLight(c), c->needsRefresh = true; }

    if (auto c = _e->get_component<PointLightComponent>()) {
        if (c->needsConfigure == true) configure_component_PointLight(c), c->needsConfigure = true;
        if (c->needsRefresh == true) refresh_component_PointLight(c), c->needsRefresh = true; }

    if (auto c = _e->get_component<ReflectComponent>()) {
        if (c->needsConfigure == true) configure_component_Reflect(c), c->needsConfigure = true;
        if (c->needsRefresh == true) refresh_component_Reflect(c), c->needsRefresh = true; }

    if (auto c = _e->get_component<DecalComponent>()) {
        if (c->needsConfigure == true) configure_component_Decal(c), c->needsConfigure = true;
        if (c->needsRefresh == true) refresh_component_Decal(c), c->needsRefresh = true; }

    for (auto& child : _e->get_children())
        configure_and_update_entity(child.get());
}


void Renderer::Implementation::configure_component_Transform(TransformComponent* _c) {}

void Renderer::Implementation::refresh_component_Transform(TransformComponent* _c) {
    _c->matrix = maths::translate(Mat4F(), _c->PROP_position);
    _c->matrix *= Mat4F(Mat3F(_c->PROP_rotation) * Mat3F(_c->PROP_scale));

    if (_c->PRNT_Transform != nullptr)
        _c->matrix = _c->PRNT_Transform->matrix * _c->matrix;
}

void Renderer::Implementation::update_component_Transform(TransformComponent* _c) {}


void Renderer::Implementation::configure_component_Model(ModelComponent* _c) {
    if ((_c->mesh = sq::static_Mesh().get(_c->PROP_mesh)) == nullptr)
        _c->mesh = sq::static_Mesh().add(_c->PROP_mesh, _c->PROP_mesh);
    if ((_c->skin = sq::static_Skin().get(_c->PROP_skin)) == nullptr)
        _c->skin = sq::static_Skin().add(_c->PROP_skin, _c->PROP_skin);

    if (_c->PROP_arma.empty() == false) {
        if ((_c->arma = sq::static_Armature().get(_c->PROP_arma)) == nullptr)
            _c->arma = sq::static_Armature().add(_c->PROP_arma, _c->PROP_arma);

        if (_c->arma->boneVector.size() * 12u != _c->ubo.get_size() - 32u) {
            _c->ubo.delete_object();
            _c->ubo.reserve("matrix", 16u);
            _c->ubo.reserve("normMat", 16u);
            _c->ubo.reserve("bones", _c->arma->boneVector.size() * 12u);
            _c->ubo.create_and_allocate();
        }
    } else if (_c->ubo.get_size() != 32u) {
        _c->ubo.delete_object();
        _c->ubo.reserve("matrix", 16u);
        _c->ubo.reserve("normMat", 16u);
        _c->ubo.create_and_allocate();
        _c->arma = nullptr;
    }
}

void Renderer::Implementation::refresh_component_Model(ModelComponent* _c) {
    _c->matrix = maths::scale(_c->DEP_Transform->matrix, _c->PROP_scale);
    _c->normMat = Mat4F(maths::transpose(maths::inverse(Mat3F(renderer.world.camera->viewMat * _c->matrix))));
    _c->bbox = sq::make_BoundBox(_c->matrix, _c->mesh->origin, _c->mesh->radius, _c->mesh->bbsize);
    _c->negScale = maths::determinant(_c->matrix) < 0.f;

    _c->ubo.update("matrix", &_c->matrix);
    _c->ubo.update("normMat", &_c->normMat);

    if (_c->arma != nullptr) {
        const auto uboData = sq::Armature::make_UboData(_c->arma->poseMap.at(_c->PROP_pose));
        _c->ubo.update("bones", uboData.data());
    }
}

void Renderer::Implementation::update_component_Model(ModelComponent* _c) {}


void Renderer::Implementation::configure_component_SpotLight(SpotLightComponent* _c) {
    if (_c->PROP_texsize == 0u) _c->tex.delete_object();
    else { uint newSize = _c->PROP_texsize * (renderer.settings.get<bool>("rpg_shadlarge") + 1u);
        if (newSize != _c->tex.get_size().x) _c->tex.allocate_storage(Vec2U(newSize), false); }
}

void Renderer::Implementation::refresh_component_SpotLight(SpotLightComponent* _c) {
    Vec3F position = Vec3F(_c->DEP_Transform->matrix[3]);
    Vec3F direction = maths::normalize(Mat3F(_c->DEP_Transform->matrix) * Vec3F(0.f, 0.f, -1.f));
    Vec3F tangent = maths::normalize(Mat3F(_c->DEP_Transform->matrix) * Vec3F(0.f, 1.f, 0.f));
    Mat4F viewMat = maths::look_at(position, position + direction, tangent);

    float angle = maths::radians(_c->PROP_angle);
    float intensity = maths::length(Vec3F(_c->DEP_Transform->matrix[0]));
    _c->matrix = maths::perspective(angle*2.f, 1.f, 0.2f, intensity) * viewMat;
    _c->frus = sq::make_Frustum(_c->matrix, position, direction, 0.2f, intensity);
    Vec3F modelScale = Vec3F(Vec2F(-std::tan(angle*2.f)), -1.f) * intensity;
    _c->modelMat = maths::scale(maths::inverse(viewMat), modelScale);

    _c->ubo.update("direction", &direction);
    _c->ubo.update("intensity", &intensity);
    _c->ubo.update("position", &position);
    _c->ubo.update("softness", &_c->PROP_softness);
    _c->ubo.update("colour", &_c->PROP_colour);
    _c->ubo.update("angle", &angle);
    _c->ubo.update("matrix", &_c->matrix);
}

void Renderer::Implementation::update_component_SpotLight(SpotLightComponent* _c) {}


void Renderer::Implementation::configure_component_PointLight(PointLightComponent* _c) {
    if (_c->PROP_texsize == 0u) _c->tex.delete_object();
    else { uint newSize = _c->PROP_texsize * (renderer.settings.get<bool>("rpg_shadlarge") + 1u);
        if (newSize != _c->tex.get_size().x) _c->tex.allocate_storage(newSize, false); }
}

void Renderer::Implementation::refresh_component_PointLight(PointLightComponent* _c) {
    Vec3F position = Vec3F(_c->DEP_Transform->matrix[3]);
    float intensity = maths::length(Vec3F(_c->DEP_Transform->matrix[0]));
    _c->modelMat = maths::scale(maths::translate(Mat4F(), position), Vec3F(intensity*2.f));
    _c->sphere.origin = position; _c->sphere.radius = intensity;

    _c->ubo.update("position", &position);
    _c->ubo.update("colour", &_c->PROP_colour);
    _c->ubo.update("intensity", &intensity);

    if (_c->PROP_texsize != 0u) {
        Mat4F projMat = maths::perspective(maths::radians(0.25f), 1.f, 0.1f, intensity);
        _c->matArr[0] = projMat * maths::look_at(position, position+Vec3F(+1.f, 0.f, 0.f), {0.f, -1.f, 0.f});
        _c->matArr[1] = projMat * maths::look_at(position, position+Vec3F(-1.f, 0.f, 0.f), {0.f, -1.f, 0.f});
        _c->matArr[2] = projMat * maths::look_at(position, position+Vec3F(0.f, +1.f, 0.f), {0.f, 0.f, +1.f});
        _c->matArr[3] = projMat * maths::look_at(position, position+Vec3F(0.f, -1.f, 0.f), {0.f, 0.f, -1.f});
        _c->matArr[4] = projMat * maths::look_at(position, position+Vec3F(0.f, 0.f, +1.f), {0.f, -1.f, 0.f});
        _c->matArr[5] = projMat * maths::look_at(position, position+Vec3F(0.f, 0.f, -1.f), {0.f, -1.f, 0.f});
        _c->frusArr[0] = sq::make_Frustum(_c->matArr[0], position, {+1.f, 0.f, 0.f}, 0.1f, intensity);
        _c->frusArr[1] = sq::make_Frustum(_c->matArr[1], position, {-1.f, 0.f, 0.f}, 0.1f, intensity);
        _c->frusArr[2] = sq::make_Frustum(_c->matArr[2], position, {0.f, +1.f, 0.f}, 0.1f, intensity);
        _c->frusArr[3] = sq::make_Frustum(_c->matArr[3], position, {0.f, -1.f, 0.f}, 0.1f, intensity);
        _c->frusArr[4] = sq::make_Frustum(_c->matArr[4], position, {0.f, 0.f, +1.f}, 0.1f, intensity);
        _c->frusArr[5] = sq::make_Frustum(_c->matArr[5], position, {0.f, 0.f, -1.f}, 0.1f, intensity);
        _c->ubo.update("matArr", _c->matArr.data());
    }
}

void Renderer::Implementation::update_component_PointLight(PointLightComponent* _c) {}


void Renderer::Implementation::configure_component_Reflect(ReflectComponent* _c) {}

void Renderer::Implementation::refresh_component_Reflect(ReflectComponent* _c) {
    _c->normal = maths::normalize(sq::make_normMat(_c->DEP_Transform->matrix) * Vec3F(0.f, 0.f, 1.f));
    _c->trans = Vec3F(_c->DEP_Transform->matrix[3]);

    _c->ubo.update("normal", &_c->normal);
    _c->ubo.update("trans", &_c->trans);
    _c->ubo.update("factor", &_c->PROP_factor);
}

void Renderer::Implementation::update_component_Reflect(ReflectComponent* _c) {}


void Renderer::Implementation::configure_component_Decal(DecalComponent* _c) {
    if (_c->PROP_diff.empty() == false) {
        const string path = "decals/" + _c->PROP_diff + "_d";
        if ((_c->texDiff = sq::static_Texture2D().get(path)) == nullptr) {
            _c->texDiff = sq::static_Texture2D().add(path, gl::RGBA, gl::RGBA8, sq::Texture::MipmapClamp());
            _c->texDiff->buffer_auto(path, true); } }
    if (_c->PROP_norm.empty() == false) {
        const string path = "decals/" + _c->PROP_norm + "_n";
        if ((_c->texNorm = sq::static_Texture2D().get(path)) == nullptr) {
            _c->texNorm = sq::static_Texture2D().add(path, gl::RGBA, gl::RGBA8, sq::Texture::MipmapClamp());
            _c->texNorm->buffer_auto(path, true); } }
    if (_c->PROP_spec.empty() == false) {
        const string path = "decals/" + _c->PROP_spec + "_s";
        if ((_c->texSpec = sq::static_Texture2D().get(path)) == nullptr) {
            _c->texSpec = sq::static_Texture2D().add(path, gl::RGBA, gl::RGBA8, sq::Texture::MipmapClamp());
            _c->texSpec->buffer_auto(path, true); } }

    Vec3I d_n_s(Vec3B(_c->texDiff, _c->texNorm, _c->texSpec));
    _c->ubo.update("d_n_s", &d_n_s);
}

void Renderer::Implementation::refresh_component_Decal(DecalComponent* _c) {
    _c->matrix = maths::scale(_c->DEP_Transform->matrix, _c->PROP_scale);
    _c->bbox = sq::make_BoundBox(_c->matrix, Vec3F(0.f), 0.87f, Vec3F(1.f));
    _c->invMat = maths::inverse(_c->matrix);

    _c->ubo.update("matrix", &_c->matrix);
    _c->ubo.update("invMat", &_c->invMat);
    _c->ubo.update("alpha", &_c->PROP_alpha);
}

void Renderer::Implementation::update_component_Decal(DecalComponent* _c) {}


void Renderer::Implementation::update_render_lists(const sq::Entity* _e) {
    auto modelC = _e->get_component<ModelComponent>();
    auto spotLightC = _e->get_component<SpotLightComponent>();
    auto pointLightC = _e->get_component<PointLightComponent>();
    auto reflectC = _e->get_component<ReflectComponent>();
    auto decalC = _e->get_component<DecalComponent>();

    if (modelC != nullptr && reflectC == nullptr && modelC->PROP_render == true)
        if (sq::bbox_in_frus(modelC->bbox, renderer.camera->frus)) {
            if (modelC->arma) renderer.cameraData.modelSkellyVec.push_back(modelC);
            else renderer.cameraData.modelSimpleVec.push_back(modelC); }

    if (modelC != nullptr && reflectC != nullptr && modelC->PROP_render == true) {
        float camOffset = maths::dot(renderer.camera->pos, reflectC->normal);
        float offset = maths::dot(-reflectC->normal, reflectC->trans);
        if (camOffset + offset > 0.f && sq::bbox_in_frus(modelC->bbox, renderer.camera->frus)) {
            renderer.reflectDataVec.emplace_back(reflectC, sq::reflect_Frustum(renderer.camera->frus, reflectC->normal, reflectC->trans));
            renderer.cameraData.modelSimpleVec.push_back(modelC); } }

    if (modelC != nullptr && modelC->PROP_shadow == true) {
        for (uint csm = 0u; csm < 4u; ++csm)
            if (sq::bbox_in_orth(modelC->bbox, renderer.world.skylight->orthArrA[csm])) {
                if (modelC->arma && modelC->skin->hasPunchThru) renderer.skyLightData.modelSkellyPunchVecArrA[csm].push_back(modelC);
                else if (modelC->skin->hasPunchThru) renderer.skyLightData.modelSimplePunchVecArrA[csm].push_back(modelC);
                else if (modelC->arma) renderer.skyLightData.modelSkellyVecArrA[csm].push_back(modelC);
                else renderer.skyLightData.modelSimpleVecArrA[csm].push_back(modelC); }

        for (uint csm = 0u; csm < 2u; ++csm)
            if (sq::bbox_in_orth(modelC->bbox, renderer.world.skylight->orthArrB[csm])) {
                if (modelC->arma && modelC->skin->hasPunchThru) renderer.skyLightData.modelSkellyPunchVecArrB[csm].push_back(modelC);
                else if (modelC->skin->hasPunchThru) renderer.skyLightData.modelSimplePunchVecArrB[csm].push_back(modelC);
                else if (modelC->arma) renderer.skyLightData.modelSkellyVecArrB[csm].push_back(modelC);
                else renderer.skyLightData.modelSimpleVecArrB[csm].push_back(modelC); }
    }

    if (spotLightC != nullptr && spotLightC->PROP_texsize != 0u)
        if (sq::frus_in_frus(spotLightC->frus, renderer.camera->frus))
            renderer.cameraData.spotLightShadowVec.push_back(spotLightC),
            visibleSpotLightSet.emplace(spotLightC);

    if (spotLightC != nullptr && spotLightC->PROP_texsize == 0u)
        if (sq::frus_in_frus(spotLightC->frus, renderer.camera->frus))
            renderer.cameraData.spotLightNoShadowVec.push_back(spotLightC);

    if (pointLightC != nullptr && pointLightC->PROP_texsize != 0u)
        if (sq::sphr_in_frus(pointLightC->sphere, renderer.camera->frus)) {
            auto& visible = visiblePointLightMap[pointLightC];
            visible[0] = sq::frus_in_frus(pointLightC->frusArr[0], renderer.camera->frus);
            visible[1] = sq::frus_in_frus(pointLightC->frusArr[1], renderer.camera->frus);
            visible[2] = sq::frus_in_frus(pointLightC->frusArr[2], renderer.camera->frus);
            visible[3] = sq::frus_in_frus(pointLightC->frusArr[3], renderer.camera->frus);
            visible[4] = sq::frus_in_frus(pointLightC->frusArr[4], renderer.camera->frus);
            visible[5] = sq::frus_in_frus(pointLightC->frusArr[5], renderer.camera->frus);
            renderer.cameraData.pointLightShadowVec.push_back(pointLightC); }

    if (pointLightC != nullptr && pointLightC->PROP_texsize == 0u)
        if (sq::sphr_in_frus(pointLightC->sphere, renderer.camera->frus))
            renderer.cameraData.pointLightNoShadowVec.push_back(pointLightC);

    if (decalC != nullptr && decalC->PROP_alpha > 0.001f)
        if (sq::bbox_in_frus(decalC->bbox, renderer.camera->frus)) {
            if (decalC->texDiff && decalC->texNorm && decalC->texSpec)
                renderer.cameraData.decalCompleteVec.push_back(decalC);
            else renderer.cameraData.decalPartialVec.push_back(decalC); }

    for (auto& child : _e->get_children())
        update_render_lists(child.get());
}


void Renderer::Implementation::update_reflect_lists(const sq::Entity* _e) {
    auto modelC = _e->get_component<ModelComponent>();
    auto spotLightC = _e->get_component<SpotLightComponent>();
    auto pointLightC = _e->get_component<PointLightComponent>();
    auto decalC = _e->get_component<DecalComponent>();

    if (modelC != nullptr && modelC->PROP_render == true)
        for (auto& data : renderer.reflectDataVec)
            if (sq::bbox_in_frus(modelC->bbox, data.frus)) {
                if (modelC->arma) data.modelSkellyVec.push_back(modelC);
                else data.modelSimpleVec.push_back(modelC); }

    if (spotLightC != nullptr && spotLightC->PROP_texsize != 0u)
        for (auto& data : renderer.reflectDataVec)
            if (sq::frus_in_frus(spotLightC->frus, data.frus))
                data.spotLightShadowVec.push_back(spotLightC),
                visibleSpotLightSet.emplace(spotLightC);

    if (spotLightC != nullptr && spotLightC->PROP_texsize == 0u)
        for (auto& data : renderer.reflectDataVec)
        if (sq::frus_in_frus(spotLightC->frus, data.frus))
            data.spotLightNoShadowVec.push_back(spotLightC);

    if (pointLightC != nullptr && pointLightC->PROP_texsize != 0u)
        for (auto& data : renderer.reflectDataVec)
            if (sq::sphr_in_frus(pointLightC->sphere, data.frus)) {
                auto& visible = visiblePointLightMap[pointLightC];
                visible[0] |= sq::frus_in_frus(pointLightC->frusArr[0], data.frus);
                visible[1] |= sq::frus_in_frus(pointLightC->frusArr[1], data.frus);
                visible[2] |= sq::frus_in_frus(pointLightC->frusArr[2], data.frus);
                visible[3] |= sq::frus_in_frus(pointLightC->frusArr[3], data.frus);
                visible[4] |= sq::frus_in_frus(pointLightC->frusArr[4], data.frus);
                visible[5] |= sq::frus_in_frus(pointLightC->frusArr[5], data.frus);
                data.pointLightShadowVec.push_back(pointLightC); }

    if (pointLightC != nullptr && pointLightC->PROP_texsize == 0u)
        for (auto& data : renderer.reflectDataVec)
            if (sq::sphr_in_frus(pointLightC->sphere, data.frus))
                data.pointLightNoShadowVec.push_back(pointLightC);

    if (decalC != nullptr && decalC->PROP_alpha > 0.001f)
        for (auto& data : renderer.reflectDataVec)
            if (sq::bbox_in_frus(decalC->bbox, data.frus))
                if (decalC->texDiff) data.decalDiffVec.push_back(decalC);

    for (auto& child : _e->get_children())
        update_reflect_lists(child.get());
}


void Renderer::Implementation::update_light_lists(const sq::Entity* _e) {
    auto modelC = _e->get_component<ModelComponent>();

    if (modelC != nullptr && modelC->PROP_shadow == true)
        for (auto& data : renderer.spotLightDataVec)
            if (sq::bbox_in_frus(modelC->bbox, data.light->frus)) {
                if (modelC->arma && modelC->skin->hasPunchThru) data.modelSkellyPunchVec.push_back(modelC);
                else if (modelC->skin->hasPunchThru) data.modelSimplePunchVec.push_back(modelC);
                else if (modelC->arma) data.modelSkellyVec.push_back(modelC);
                else data.modelSimpleVec.push_back(modelC); }

    if (modelC != nullptr && modelC->PROP_shadow == true)
        for (auto& data : renderer.pointLightDataVec)
            for (uint face = 0u; face < 6u; ++face)
                if (data.visibleFaceArr[face] && sq::bbox_in_frus(modelC->bbox, data.light->frusArr[face])) {
                    if (modelC->arma && modelC->skin->hasPunchThru) data.modelSkellyPunchVecArr[face].push_back(modelC);
                    else if (modelC->skin->hasPunchThru) data.modelSimplePunchVecArr[face].push_back(modelC);
                    else if (modelC->arma) data.modelSkellyVecArr[face].push_back(modelC);
                    else data.modelSimpleVecArr[face].push_back(modelC); }

    for (auto& child : _e->get_children())
        update_light_lists(child.get());
}


void Renderer::Implementation::sort_render_lists() {
    const auto cameraCompareFunc = [this](const ModelComponent* a, const ModelComponent* b) {
        return maths::distance(renderer.camera->pos, a->bbox.origin) <
               maths::distance(renderer.camera->pos, b->bbox.origin); };

    sort_container_by(renderer.cameraData.modelSimpleVec, cameraCompareFunc);
    sort_container_by(renderer.cameraData.modelSkellyVec, cameraCompareFunc);

    const auto skyLightCompareFunc = [this](const ModelComponent* a, const ModelComponent* b) {
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
        const auto spotLightCompareFunc = [&](const ModelComponent* a, const ModelComponent* b) {
            return maths::distance(data.light->frus.points[0], a->bbox.origin) <
                   maths::distance(data.light->frus.points[0], b->bbox.origin); };

        sort_container_by(data.modelSimpleVec, spotLightCompareFunc);
        sort_container_by(data.modelSimplePunchVec, spotLightCompareFunc);
        sort_container_by(data.modelSkellyVec, spotLightCompareFunc);
        sort_container_by(data.modelSkellyPunchVec, spotLightCompareFunc);
    }

    for (auto& data : renderer.pointLightDataVec) {
        for (uint face = 0u; face < 6u; ++face) {
            const auto pointLightCompareFunc = [&](const ModelComponent* a, const ModelComponent* b) {
                return maths::distance(data.light->sphere.origin, a->bbox.origin) <
                       maths::distance(data.light->sphere.origin, b->bbox.origin); };

            sort_container_by(data.modelSimpleVecArr[face], pointLightCompareFunc);
            sort_container_by(data.modelSimplePunchVecArr[face], pointLightCompareFunc);
            sort_container_by(data.modelSkellyVecArr[face], pointLightCompareFunc);
            sort_container_by(data.modelSkellyPunchVecArr[face], pointLightCompareFunc);
        }
    }

    for (auto& data : renderer.reflectDataVec) {
        const auto reflectCompareFunc = [&](const ModelComponent* a, const ModelComponent* b) {
            return maths::distance(renderer.camera->pos, maths::reflect(a->bbox.origin, data.reflect->normal, data.reflect->trans)) <
                   maths::distance(renderer.camera->pos, maths::reflect(a->bbox.origin, data.reflect->normal, data.reflect->trans)); };

        sort_container_by(data.modelSimpleVec, reflectCompareFunc);
        sort_container_by(data.modelSkellyVec, reflectCompareFunc);
    }
}


void Renderer::prepare_render_stuff() {
    cameraData = CameraData();
    skyLightData = SkyLightData();
    impl->visibleSpotLightSet.clear();
    impl->visiblePointLightMap.clear();
    spotLightDataVec.clear();
    pointLightDataVec.clear();
    reflectDataVec.clear();

    impl->configure_and_update_entity(&world.root);

    impl->update_render_lists(&world.root);

    impl->update_reflect_lists(&world.root);

    for (auto light : impl->visibleSpotLightSet)
        spotLightDataVec.emplace_back(light);

    for (const auto& light : impl->visiblePointLightMap)
        pointLightDataVec.emplace_back(light.first, light.second);

    impl->update_light_lists(&world.root);

    impl->sort_render_lists();
}


Renderer::Renderer(sq::MessageBus& _messageBus, const sq::Settings& _settings,
                   const sq::PreProcessor& _preprocs, const sq::Pipeline& _pipeline,
                   wcoe::World& _world)
    : messageBus(_messageBus), settings(_settings), preprocs(_preprocs),
      pipeline(_pipeline), world(_world), impl(new Implementation(*this)) {

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

    impl->configure_and_update_entity(&world.root);

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
