#include <sqee/dop/Functions.hpp>

#include "../world/World.hpp"

#include "../systems/Render.hpp"
#include "../systems/Culling.hpp"

#include "ObjectsData.hpp"
#include "PassesData.hpp"

namespace dop = sq::dop;
namespace maths = sq::maths;
using namespace sqt::render;

//============================================================================//

void PassesData::prepare(const ObjectsData& objects)
{
    // Clear Main Depth Passes
    depthData.modelSimplePass.solidFullVec.clear();
    depthData.modelSimplePass.solidPartVec.clear();
    depthData.modelSimplePass.punchPartMap.clear();
    depthData.modelSkellyPass.solidFullVec.clear();
    depthData.modelSkellyPass.solidPartVec.clear();
    depthData.modelSkellyPass.punchPartMap.clear();

    // Clear Main Gbuffer Passes
    gbufferData.modelPasses.simplePass.baseVec.clear();
    gbufferData.modelPasses.simplePass.partMap.clear();
    gbufferData.modelPasses.skellyPass.baseVec.clear();
    gbufferData.modelPasses.skellyPass.partMap.clear();
    gbufferData.decalPasses.basicPass.decalMap.clear();

    // Clear Shadow Map Passes
    shadowsData.cascPassVec.clear();
    shadowsData.orthoPassVec.clear();
    shadowsData.pointPassVec.clear();
    shadowsData.spotPassVec.clear();

    // Clear Light Base Passes
    lightBaseData.skyboxPass.reset();
    lightBaseData.ambientPass.reset();

    // Clear Light Accum Passes
    lightAccumData.skylightPass.reset();
    lightAccumData.orthoPassVec.clear();
    lightAccumData.pointPassVec.clear();
    lightAccumData.spotPassVec.clear();

    // Clear Volumetric Passes
    volumetricData.skylightPass.reset();
    volumetricData.orthoPassVec.clear();
    volumetricData.pointPassVec.clear();
    volumetricData.spotPassVec.clear();

    //========================================================//

    // Clear Visible Light Sets
//    visibleOrthoSet.clear();
//    visiblePointMap.clear();
//    visibleSpotSet.clear();


    const auto& stuff = sys::static_WorldStuff();

    const auto& camera = *objects.cameraData;

    mVisibleSet = sys::system_get_visible_objects_frustum(stuff, camera.frustum);

    mModelSimpleSet = dop::reduce(mVisibleSet, stuff.groups.modelSimple);
    mModelSkellySet = dop::reduce(mVisibleSet, stuff.groups.modelSkelly);
    mLightSpotSet = dop::reduce(mVisibleSet, stuff.groups.lightSpot);

    prepare_depth_models(objects);

    prepare_gbuffer_models(objects);
    prepare_gbuffer_decals(objects);

    prepare_lighting_basic(objects);
    prepare_lighting_accum(objects);

    prepare_shadow_mapping(objects);
}

//============================================================================//

#define SQ_BEGIN_FOR_EACH_ModelSimple \
    for (const auto& entityDataPair : _objects.entityMap) { \
        if (entityDataPair.second.modelSimple != nullptr) { \
            const ModelSimpleData& model = *entityDataPair.second.modelSimple;

#define SQ_BEGIN_FOR_EACH_ModelSkelly \
    for (const auto& entityDataPair : _objects.entityMap) { \
        if (entityDataPair.second.modelSkelly != nullptr) { \
            const ModelSkellyData& model = *entityDataPair.second.modelSkelly;

#define SQ_BEGIN_FOR_EACH_DecalBasic \
    for (const auto& entityDataPair : _objects.entityMap) { \
        if (entityDataPair.second.decalBasic != nullptr) { \
            const DecalBasicData& decal = *entityDataPair.second.decalBasic;

#define SQ_BEGIN_FOR_EACH_LightOrtho \
    for (const auto& entityDataPair : _objects.entityMap) { \
        if (entityDataPair.second.lightOrtho != nullptr) { \
            const LightOrthoData& light = *entityDataPair.second.lightOrtho;

#define SQ_BEGIN_FOR_EACH_LightPoint \
    for (const auto& entityDataPair : _objects.entityMap) { \
        if (entityDataPair.second.lightPoint != nullptr) { \
            const LightPointData& light = *entityDataPair.second.lightPoint;

#define SQ_BEGIN_FOR_EACH_LightSpot \
    for (const auto& entityDataPair : _objects.entityMap) { \
        if (entityDataPair.second.lightSpot != nullptr) { \
            const LightSpotData& light = *entityDataPair.second.lightSpot;

#define SQ_END_FOR_EACH }}

//============================================================================//

void PassesData::prepare_depth_models(const ObjectsData& objects)
{
    auto& simplePass = depthData.modelSimplePass;
    auto& skellyPass = depthData.modelSkellyPass;

    const CameraData& camera = *objects.cameraData;

    //========================================================//

    const auto& modelTable = objects.mModelTable;
    const auto& skeletonTable = objects.mSkeletonTable;

    const auto refs_ModelSimple = dop::joined(mModelSimpleSet, modelTable);
    const auto refs_ModelSkelly = dop::joined(mModelSkellySet, modelTable, skeletonTable);

    //========================================================//

    for (auto& entry : refs_ModelSimple)
    {
        const auto& model = std::get<1>(entry);

        // calculate model-view-projection matrix for this model
        const Mat4F matrix = camera.projMat * camera.viewMat * model.modelMatrix;

        // check if this model has any mask textures
        if (model.hasMaskTexture == true)
        {
            // iterate through sub-meshes and get mask handles
            for (uint index = 0u; index < model.materials.size(); ++index)
            {
                const auto& mask = model.materials[index]->get_mask_handle();

                // add this sub-mesh either to the punch part map, or the solid part vector
                auto& dest = mask.check() ? simplePass.punchPartMap[&mask.get()] : simplePass.solidPartVec;
                dest.push_back({model.mesh.get(), matrix, model.isMirrored, index});
        }}

        // no sub-meshes have a mask texture, so add to the full model vector
        else simplePass.solidFullVec.push_back({model.mesh.get(), matrix, model.isMirrored});
    }

    //========================================================//

    for (auto& entry : refs_ModelSkelly)
    {
        const auto& model = std::get<1>(entry);
        const auto& skeleton = std::get<2>(entry);

        // calculate model-view-projection matrix for this model
        const Mat4F matrix = camera.projMat * camera.viewMat * model.modelMatrix;

        // check if this model has any mask textures
        if (model.hasMaskTexture == true)
        {
            // iterate through sub-meshes and get mask handles
            for (uint index = 0u; index < model.materials.size(); ++index)
            {
                const auto& mask = model.materials[index]->get_mask_handle();

                // add this sub-mesh either to the punch part map, or the solid part vector
                auto& dest = mask.check() ? skellyPass.punchPartMap[&mask.get()] : skellyPass.solidPartVec;
                dest.push_back({skeleton.ubo, model.mesh.get(), matrix, model.isMirrored, index});
        }}

        // no sub-meshes have a mask texture, so add to the full model vector
        else skellyPass.solidFullVec.push_back({skeleton.ubo, model.mesh.get(), matrix, model.isMirrored});
    }
}

//============================================================================//

void PassesData::prepare_gbuffer_models(const ObjectsData& objects)
{
    auto& simplePass = gbufferData.modelPasses.simplePass;
    auto& skellyPass = gbufferData.modelPasses.skellyPass;

    const CameraData& camera = *objects.cameraData;

    //========================================================//

    const auto& modelTable = objects.mModelTable;
    const auto& skeletonTable = objects.mSkeletonTable;

    const auto refs_ModelSimple = dop::joined(mModelSimpleSet, modelTable);
    const auto refs_ModelSkelly = dop::joined(mModelSkellySet, modelTable, skeletonTable);

    //========================================================//

    for (auto& entry : refs_ModelSimple)
    {
        const auto& model = std::get<1>(entry);

        // calculate model-view-projection and normal matrices
        const Mat4F matrix = camera.projMat * camera.viewMat * model.modelMatrix;
        const Mat3F normMat = maths::normal_matrix(camera.viewMat * model.modelMatrix);

        // add shared base data to the gbuffer model simple pass
        simplePass.baseVec.push_back({model.mesh.get(), matrix, normMat, model.isMirrored});
        const auto& base = simplePass.baseVec.back();

        // add each sub-mesh to the gbuffer model simple pass
        for (uint index = 0u; index < model.materials.size(); ++index)
        {
            const auto* const material = &model.materials[index].get();
            simplePass.partMap[material].push_back({base, index});
        }
    }

    //========================================================//

    for (auto& entry : refs_ModelSkelly)
    {
        const auto& model = std::get<1>(entry);
        const auto& skeleton = std::get<2>(entry);

        // calculate model-view-projection and normal matrices
        const Mat4F matrix = camera.projMat * camera.viewMat * model.modelMatrix;
        const Mat3F normMat = maths::normal_matrix(camera.viewMat * model.modelMatrix);

        // add shared base data to the gbuffer model skelly pass
        skellyPass.baseVec.push_back({skeleton.ubo, model.mesh.get(), matrix, normMat, model.isMirrored});
        const auto& base = skellyPass.baseVec.back();

        // add each sub-mesh to the gbuffer model skelly pass
        for (uint index = 0u; index < model.materials.size(); ++index)
        {
            const auto* const material = &model.materials[index].get();
            skellyPass.partMap[material].push_back({base, index});
        }
    }
}

//============================================================================//

void PassesData::prepare_gbuffer_decals(const ObjectsData& _objects) {

    auto& basicPass = gbufferData.decalPasses.basicPass;

    const CameraData& camera = *_objects.cameraData;

/*    SQ_BEGIN_FOR_EACH_DecalBasic {

        // Check if decal is visible to main camera
        if (sq::intersects(decal.bbox, camera.frus) == true) {

            // calculate model-view matrix and it's inverse
            const Mat4F matrix = camera.viewMat * decal.modelMat;
            const Mat4F invMatrix = maths::inverse(matrix);

            // Add the decal to the correct vector in the gbuffer decal basic pass map
            basicPass.decalMap[&decal.material.get()].push_back({decal.alpha, matrix, invMatrix});
        }

    } SQ_END_FOR_EACH;*/
}

//============================================================================//

void PassesData::prepare_lighting_basic(const ObjectsData& objects)
{
    const auto& skybox = objects.skyboxData;
    const auto& ambient = objects.ambientData;

    auto& skyboxPass = lightBaseData.skyboxPass;
    auto& ambientPass = lightBaseData.ambientPass;

    if (skybox != nullptr) skyboxPass.reset(new data::LightBaseSkyBoxPass{skybox->tex, skybox->params});
    if (ambient != nullptr) ambientPass.reset(new data::LightBaseAmbientPass{ambient->colour});
}

//============================================================================//

void PassesData::prepare_lighting_accum(const ObjectsData& objects)
{
    auto& skylightPass = lightAccumData.skylightPass;
    auto& orthoPassVec = lightAccumData.orthoPassVec;
    auto& pointPassVec = lightAccumData.pointPassVec;
    auto& spotPassVec = lightAccumData.spotPassVec;

    auto& skylightShafts = volumetricData.skylightPass;
    auto& orthoShaftsVec = volumetricData.orthoPassVec;
    auto& pointShaftsVec = volumetricData.pointPassVec;
    auto& spotShaftsVec = volumetricData.spotPassVec;

    //========================================================//

    auto& simplePass = depthData.modelSimplePass;
    auto& skellyPass = depthData.modelSkellyPass;

    const CameraData& camera = *objects.cameraData;

    //========================================================//

    const auto& culling = sys::static_WorldStuff().culling;

    //========================================================//

    if (objects.skylightData != nullptr)
    {
        const SkyLightData& skylight = *objects.skylightData;
        skylightPass.reset(new data::LightAccumSkyLightPass{skylight.ubo, skylight.tex});
        skylightShafts.reset(new data::VolumetricSkyLightPass{skylight.ubo, skylight.tex});
    }
/*
    for (auto& entry : dop::joined(mLightSpotSet, objects.mLightSpotTable, culling.spotLights))
    {
        const auto& light = std::get<1>(entry);
        const auto& cullData = std::get<2>(entry);

        const Vec2U viewport = Vec2U(light.tex.get_size());

        // iterate through individual faces
        for (uint face = 0u; face < 6u; ++face)
        {
            // skip this face if it is not visible
            if (visibleFaces[face] == false) continue;
            const auto& cascade = light.cascades[ind];

            // add the current cascade to the pass vector
            cascPassVec.push_back({cascade.fbo, viewPort, {}});

            // get the complete set of entities visible to this cascade
            auto visibleSet = mCullingSys->get_visible_set(cascade.planes);

            // reduce the visible set down to sets for each type of entity
            auto modelSimpleSet = dop::reduce(visibleSet, mRenderSys->get_ModelSimpleSet());
            auto modelSkellySet = dop::reduce(visibleSet, mRenderSys->get_ModelSkellySet());

            add_simple_models_to_shadows(cascPassVec.back().depthPasses.modelSimplePass,
                                         objects, modelSimpleSet, cascade.matrix);

            add_skelly_models_to_shadows(cascPassVec.back().depthPasses.modelSkellyPass,
                                         objects, modelSkellySet, cascade.matrix);
        }
    }

    SQ_BEGIN_FOR_EACH_LightPoint {

        // Check if this light is visible to main camera
        if (sq::intersects(light.sphere, camera.frus) == true) {

            // Calculate model-view-projection matrix for this light
            const Mat4F matrix = camera.projMat * camera.viewMat * light.modelMat;

            // Check if the camera is inside the light volume
            bool inside = sq::point_in_sphere_volume(camera.position, light.sphere);

            // Add this point light to the main light passes
            pointPassVec.push_back({light.ubo, light.tex, matrix});
            pointShaftsVec.push_back({light.ubo, light.tex, matrix, !inside});

            // Add this point light to the visible map
            array<bool, 6>& visFaces = visiblePointMap[entityDataPair.first];

            // Mark each face of the point light as either visible or not visible
            for (uint face = 0u; face < 6u; ++face) visFaces[face] = sq::intersects(light.faces[face].frus, camera.frus);
        }

    } SQ_END_FOR_EACH;
*/
/*    SQ_BEGIN_FOR_EACH_LightOrtho {

        // Check if this light is visible to main camera
        //if (sq::bbox_in_frus(light.bbox, camera.frus) == true) {

            // Calculate model-view-projection matrix for this light
            const Mat4F matrix = camera.projMat * camera.viewMat * light.modelMat;

            // Add this ortho light to the main light passes
            orthoPassVec.push_back({light.ubo, light.tex, matrix});
            //orthoShaftsVec.push_back({light.ubo, light.tex, matrix, false});

            // Mark this ortho light to render shadows
            visibleOrthoSet.insert(entityDataPair.first);
        //}

    } SQ_END_FOR_EACH;*/

/*    SQ_BEGIN_FOR_EACH_LightPoint {

        // Check if this light is visible to main camera
        if (sq::intersects(light.sphere, camera.frus) == true) {

            // Calculate model-view-projection matrix for this light
            const Mat4F matrix = camera.projMat * camera.viewMat * light.modelMat;

            // Check if the camera is inside the light volume
            bool inside = sq::point_in_sphere_volume(camera.position, light.sphere);

            // Add this point light to the main light passes
            pointPassVec.push_back({light.ubo, light.tex, matrix});
            pointShaftsVec.push_back({light.ubo, light.tex, matrix, !inside});

            // Add this point light to the visible map
            array<bool, 6>& visFaces = visiblePointMap[entityDataPair.first];

            // Mark each face of the point light as either visible or not visible
            for (uint face = 0u; face < 6u; ++face) visFaces[face] = sq::intersects(light.faces[face].frus, camera.frus);
        }

    } SQ_END_FOR_EACH;*/

/*    SQ_BEGIN_FOR_EACH_LightSpot {

        // Check if this light is visible to main camera
        if (sq::intersects(light.frus, camera.frus) == true) {

            // Calculate model-view-projection matrix for this light
            const Mat4F matrix = camera.projMat * camera.viewMat * light.modelMat;

            // Check if the camera is inside the light volume
            bool inside = sq::point_in_cone_volume(camera.position, light.frus);

            // Add this spot light to the main light passes
            spotPassVec.push_back({light.ubo, light.tex, matrix});
            spotShaftsVec.push_back({light.ubo, light.tex, matrix, !inside});

            // Mark this spot light to render shadows
            visibleSpotSet.insert(entityDataPair.first);
        }

    } SQ_END_FOR_EACH;*/
}

//============================================================================//

namespace { // anonymous

inline void add_simple_models_to_shadows(data::DepthModelSimplePass& pass, const ObjectsData& objects,
                                         const dop::Group& group, const Mat4F& lightMatrix)
{
    // iterate through all simple models
    for (auto& entry : dop::joined(group, objects.mModelTable))
    {
        const auto& model = std::get<1>(entry);

        // calculate combined matrix for this model
        const Mat4F matrix = lightMatrix * model.modelMatrix;

        // check if this model has any mask textures
        if (model.hasMaskTexture == true)
        {
            // iterate through sub-meshes and get mask handles
            for (uint index = 0u; index < model.materials.size(); ++index)
            {
                const auto& mask = model.materials[index]->get_mask_handle();

                // add this sub-mesh either to the punch part map, or the solid part vector
                auto& dest = mask.check() ? pass.punchPartMap[&mask.get()] : pass.solidPartVec;
                dest.push_back({model.mesh.get(), matrix, model.isMirrored, index});
        }}

        // no sub-meshes have a mask texture, so add to the full model vector
        else pass.solidFullVec.push_back({model.mesh.get(), matrix, model.isMirrored});
    }
}

inline void add_skelly_models_to_shadows(data::DepthModelSkellyPass& pass, const ObjectsData& objects,
                                         const dop::Group& group, const Mat4F& lightMatrix)
{
    // iterate through all skelly models
    for (auto& entry : dop::joined(group, objects.mModelTable, objects.mSkeletonTable))
    {
        const auto& model = std::get<1>(entry);
        const auto& skeleton = std::get<2>(entry);

        // calculate combined matrix for this model
        const Mat4F matrix = lightMatrix * model.modelMatrix;

        // check if this model has any mask textures
        if (model.hasMaskTexture == true)
        {
            // iterate through sub-meshes and get mask handles
            for (uint index = 0u; index < model.materials.size(); ++index)
            {
                const auto& mask = model.materials[index]->get_mask_handle();

                // add this sub-mesh either to the punch part map, or the solid part vector
                auto& dest = mask.check() ? pass.punchPartMap[&mask.get()] : pass.solidPartVec;
                dest.push_back({skeleton.ubo, model.mesh.get(), matrix, model.isMirrored, index});
        }}

        // no sub-meshes have a mask texture, so add to the full model vector
        else pass.solidFullVec.push_back({skeleton.ubo, model.mesh.get(), matrix, model.isMirrored});
    }
}

} // anonymous

//============================================================================//

void PassesData::prepare_shadow_mapping(const ObjectsData& objects)
{
    const auto& stuff = sys::static_WorldStuff();

    auto& cascPassVec = shadowsData.cascPassVec;
    auto& orthoPassVec = shadowsData.orthoPassVec;
    auto& pointPassVec = shadowsData.pointPassVec;
    auto& spotPassVec = shadowsData.spotPassVec;

    if (objects.skylightData != nullptr)
    {
        const SkyLightData& light = *objects.skylightData;
        const Vec2U viewPort = Vec2U(light.tex.get_size());

        // iterate through individual cascades
        for (uint ind = 0u; ind < light.cascadeCount; ++ind)
        {
            const auto& cascade = light.cascades[ind];

            // add the current cascade to the pass vector
            cascPassVec.push_back({cascade.fbo, viewPort, {}});

            // get the complete set of entities visible to this cascade
            auto visibleSet = sys::system_get_visible_objects_planes(stuff, cascade.planes);
            //auto visibleSet = mCullingSys->get_visible_entity_set(cascade.planes);

            // reduce the visible set down to sets for each type of entity
            auto modelSimpleSet = dop::reduce(visibleSet, stuff.groups.modelSimple);
            auto modelSkellySet = dop::reduce(visibleSet, stuff.groups.modelSkelly);

            add_simple_models_to_shadows(cascPassVec.back().depthPasses.modelSimplePass,
                                         objects, modelSimpleSet, cascade.matrix);

            add_skelly_models_to_shadows(cascPassVec.back().depthPasses.modelSkellyPass,
                                         objects, modelSkellySet, cascade.matrix);
        }
    }

    //========================================================//

//    for (auto& entry : dop::joined(mLightSpotSet, objects.mLightSpotTable))
//    {
//        const auto& light = std::get<1>(entry);
//        const Vec2U viewport = Vec2U(light.tex.get_size());

//        // iterate through individual faces
//        for (uint face = 0u; face < 6u; ++face)
//        {
//            // skip this face if it is not visible
//            if (visibleFaces[face] == false) continue;
//            const auto& cascade = light.cascades[ind];

//            // add the current cascade to the pass vector
//            cascPassVec.push_back({cascade.fbo, viewPort, {}});

//            // get the complete set of entities visible to this cascade
//            auto visibleSet = mCullingSys->get_visible_set(cascade.planes);

//            // reduce the visible set down to sets for each type of entity
//            auto modelSimpleSet = dop::reduce(visibleSet, mRenderSys->get_ModelSimpleSet());
//            auto modelSkellySet = dop::reduce(visibleSet, mRenderSys->get_ModelSkellySet());

//            add_simple_models_to_shadows(cascPassVec.back().depthPasses.modelSimplePass,
//                                         objects, modelSimpleSet, cascade.matrix);

//            add_skelly_models_to_shadows(cascPassVec.back().depthPasses.modelSkellyPass,
//                                         objects, modelSkellySet, cascade.matrix);
//        }
//    }

/*
    // iterate through visible ortho lights
    for (const sq::Entity* lightEntity : visibleOrthoSet) {

        // reference to the ortho light rendering data
        const LightOrthoData& light = *entityMap.at(lightEntity).lightOrtho;
        const Vec2U viewPort = Vec2U(light.tex.get_size());

        // add the current ortho light to the pass vector
        orthoPassVec.push_back({light.fbo, viewPort, {}});

        // get references to the model simple and model skelly passes
        auto& simplePass = orthoPassVec.back().depthPasses.modelSimplePass;
        auto& skellyPass = orthoPassVec.back().depthPasses.modelSkellyPass;

        SQ_BEGIN_FOR_EACH_StaticModel {
            if (sq::intersects(model.bbox, light.ortho) == true)
                add_StaticModel_to_ShadowsPass(simplePass, model, light.matrix);
        } SQ_END_FOR_EACH;

        SQ_BEGIN_FOR_EACH_ModelSimple {
            if (sq::intersects(model.bbox, light.ortho) == true)
                add_ModelSimple_to_ShadowsPass(simplePass, model, light.matrix);
        } SQ_END_FOR_EACH;

        SQ_BEGIN_FOR_EACH_ModelSkelly {
            if (sq::intersects(model.sphere, light.ortho) == true)
                add_ModelSkelly_to_ShadowsPass(skellyPass, model, light.matrix);
        } SQ_END_FOR_EACH;
    }

    // iterate through visible point lights
    for (const auto& lightPair : visiblePointMap) {

        // get references to map pair data
        const sq::Entity* lightEntity = lightPair.first;
        const array<bool, 6>& visibleFaces = lightPair.second;

        // reference to the point light rendering data
        const LightPointData& light = *entityMap.at(lightEntity).lightPoint;
        const Vec2U viewPort = Vec2U(light.tex.get_size());

        // iterate through faces of the cube
        for (uint face = 0u; face < 6u; ++face) {

            // skip this face if it is not visible
            if (visibleFaces[face] == false) continue;

            // add the current point light face to the pass vector
            pointPassVec.push_back({light.faces[face].fbo, viewPort, {}});

            // get references to the model simple and model skelly passes
            auto& simplePass = pointPassVec.back().depthPasses.modelSimplePass;
            auto& skellyPass = pointPassVec.back().depthPasses.modelSkellyPass;

            SQ_BEGIN_FOR_EACH_StaticModel {
                if (sq::intersects(model.bbox, light.faces[face].frus) == true)
                    add_StaticModel_to_ShadowsPass(simplePass, model, light.faces[face].matrix);
            } SQ_END_FOR_EACH;

            SQ_BEGIN_FOR_EACH_ModelSimple {
                if (sq::intersects(model.bbox, light.faces[face].frus) == true)
                    add_ModelSimple_to_ShadowsPass(simplePass, model, light.faces[face].matrix);
            } SQ_END_FOR_EACH;

            SQ_BEGIN_FOR_EACH_ModelSkelly {
                if (sq::intersects(model.sphere, light.faces[face].frus) == true)
                    add_ModelSkelly_to_ShadowsPass(skellyPass, model, light.faces[face].matrix);
            } SQ_END_FOR_EACH;
        }
    }

    // iterate through visible spot lights
    for (const sq::Entity* lightEntity : visibleSpotSet) {

        // reference to the spot light rendering data
        const LightSpotData& light = *entityMap.at(lightEntity).lightSpot;
        const Vec2U viewPort = Vec2U(light.tex.get_size());

        // add the current spot light to the pass vector
        spotPassVec.push_back({light.fbo, viewPort, {}});

        // get references to the model simple and model skelly passes
        auto& simplePass = spotPassVec.back().depthPasses.modelSimplePass;
        auto& skellyPass = spotPassVec.back().depthPasses.modelSkellyPass;

        SQ_BEGIN_FOR_EACH_StaticModel {
            if (sq::intersects(model.bbox, light.frus) == true)
                add_StaticModel_to_ShadowsPass(simplePass, model, light.matrix);
        } SQ_END_FOR_EACH;

        SQ_BEGIN_FOR_EACH_ModelSimple {
            if (sq::intersects(model.bbox, light.frus) == true)
                add_ModelSimple_to_ShadowsPass(simplePass, model, light.matrix);
        } SQ_END_FOR_EACH;

        SQ_BEGIN_FOR_EACH_ModelSkelly {
            if (sq::intersects(model.sphere, light.frus) == true)
                add_ModelSkelly_to_ShadowsPass(skellyPass, model, light.matrix);
        } SQ_END_FOR_EACH;
    }*/
}
