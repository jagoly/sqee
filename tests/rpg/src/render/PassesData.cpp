#include <sqee/core/Utilities.hpp>

#include <sqee/misc/DopFunctions.hpp>

#include "../systems/Render.hpp"
#include "../systems/Culling.hpp"

#include "../systems/WorldStuff.hpp"

#include "ObjectsData.hpp"
#include "PassesData.hpp"

namespace dop = sq::dop;
namespace maths = sq::maths;
using namespace sqt::render;
using namespace sqt;

//============================================================================//

void PassesData::prepare(const WorldStuff& wstuff, const RenderStuff& rstuff)
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

    //--------------------------------------------------------//

    // Clear Visible Light Sets
    //visibleOrthoSet.clear();
    //visiblePointMap.clear();
    //visibleSpotSet.clear();

    mVisibleSet = sys::system_get_visible_objects_frustum(wstuff, rstuff.camera.frustum);

    mModelSimpleSet = dop::reduce(mVisibleSet, wstuff.groups.modelSimple);
    mModelSkellySet = dop::reduce(mVisibleSet, wstuff.groups.modelSkelly);

    mLightOrthoSet.mIds = wstuff.groups.lightOrtho.mIds;
    mLightPointSet.mIds = wstuff.groups.lightPoint.mIds;
    mLightSpotSet.mIds = wstuff.groups.lightSpot.mIds;

    //--------------------------------------------------------//

    prepare_depth_models(rstuff);

    prepare_gbuffer_models(rstuff);
    prepare_gbuffer_decals(rstuff);

    prepare_lighting_basic(rstuff);
    prepare_lighting_accum(rstuff);

    prepare_shadow_mapping(rstuff, wstuff);
}

//============================================================================//

void PassesData::prepare_depth_models(const RenderStuff& stuff)
{
    const auto& camera = stuff.camera;

    auto& simplePass = depthData.modelSimplePass;
    auto& skellyPass = depthData.modelSkellyPass;

    //--------------------------------------------------------//

    for (auto& [id, model] : dop::joined(mModelSimpleSet, stuff.tables.model))
    {
        const Mat4F matrix = camera.projMat * camera.viewMat * model.modelMatrix;

        if (model.hasMaskTexture == true)
        {
            for (uint index = 0u; index < model.materials.size(); ++index)
            {
//                const auto& mask = model.materials[index]->get_mask_handle();

//                // add this sub-mesh either to the punch part map, or the solid part vector
//                auto& dest = mask != nullptr ? simplePass.punchPartMap[&mask.get()] : simplePass.solidPartVec;
//                dest.push_back({model.mesh.get(), matrix, model.isMirrored, index});
            }
        }

        // no sub-meshes have a mask texture, so add to the full model vector
        else simplePass.solidFullVec.push_back({model.mesh.get(), matrix, model.isMirrored});
    }

    //--------------------------------------------------------//

    for (auto& [id, model, skeleton] : dop::joined(mModelSkellySet, stuff.tables.model, stuff.tables.skeleton))
    {
        const Mat4F matrix = camera.projMat * camera.viewMat * model.modelMatrix;

        if (model.hasMaskTexture == true)
        {
            for (uint index = 0u; index < model.materials.size(); ++index)
            {
//                const auto& mask = model.materials[index]->get_mask_handle();

//                // add this sub-mesh either to the punch part map, or the solid part vector
//                auto& dest = mask != nullptr ? skellyPass.punchPartMap[&mask.get()] : skellyPass.solidPartVec;
//                dest.push_back({skeleton.ubo, model.mesh.get(), matrix, model.isMirrored, index});
            }
        }

        // no sub-meshes have a mask texture, so add to the full model vector
        else skellyPass.solidFullVec.push_back({skeleton.ubo, model.mesh.get(), matrix, model.isMirrored});
    }
}

//============================================================================//

void PassesData::prepare_gbuffer_models(const RenderStuff& stuff)
{
    const auto& camera = stuff.camera;

    auto& simplePass = gbufferData.modelPasses.simplePass;
    auto& skellyPass = gbufferData.modelPasses.skellyPass;

    //--------------------------------------------------------//

    for (auto& [id, model] : dop::joined(mModelSimpleSet, stuff.tables.model))
    {
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

    //--------------------------------------------------------//

    for (auto& [id, model, skeleton] : dop::joined(mModelSkellySet, stuff.tables.model, stuff.tables.skeleton))
    {
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

void PassesData::prepare_gbuffer_decals(const RenderStuff& stuff) {

    auto& basicPass = gbufferData.decalPasses.basicPass;

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

void PassesData::prepare_lighting_basic(const RenderStuff& stuff)
{
    const auto& skybox = stuff.skybox;
    const auto& ambient = stuff.ambient;

    auto& skyboxPass = lightBaseData.skyboxPass;
    auto& ambientPass = lightBaseData.ambientPass;

    if (skybox != nullptr) skyboxPass.reset(new data::LightBaseSkyBoxPass{skybox->tex, skybox->params});
    if (ambient != nullptr) ambientPass.reset(new data::LightBaseAmbientPass{ambient->colour});
}

//============================================================================//

void PassesData::prepare_lighting_accum(const RenderStuff& stuff)
{
    auto& skylightPass = lightAccumData.skylightPass;
    auto& orthoPassVec = lightAccumData.orthoPassVec;
    auto& pointPassVec = lightAccumData.pointPassVec;
    auto& spotPassVec = lightAccumData.spotPassVec;

    auto& skylightShafts = volumetricData.skylightPass;
    auto& orthoShaftsVec = volumetricData.orthoPassVec;
    auto& pointShaftsVec = volumetricData.pointPassVec;
    auto& spotShaftsVec = volumetricData.spotPassVec;

    //--------------------------------------------------------//

    if (auto skylight = stuff.skylight.get())
    {
        // todo: really don't need this special helper function in sqee
        skylightPass = sq::make_unique_aggregate<data::LightAccumSkyLightPass>(skylight->ubo, skylight->tex);
        skylightShafts = sq::make_unique_aggregate<data::VolumetricSkyLightPass>(skylight->ubo, skylight->tex);
    }

    //--------------------------------------------------------//

    for (auto& [id, light] : dop::joined(mLightOrthoSet, stuff.tables.ortholight))
    {
        // Calculate model-view-projection matrix for this light
        const Mat4F matrix = stuff.camera.projMat * stuff.camera.viewMat * light.modelMatrix;

        // Check if the camera is inside the light volume
        //bool inside = sq::point_in_cone_volume(camera.position, light.frus);

        // Add this ortho light to the main light passes
        orthoPassVec.push_back({light.ubo, light.tex, matrix});
        orthoShaftsVec.push_back({light.ubo, light.tex, matrix, false});
    }

    //--------------------------------------------------------//

    for (auto& [id, light] : dop::joined(mLightPointSet, stuff.tables.pointlight))
    {
        // Calculate model-view-projection matrix for this light
        const Mat4F matrix = stuff.camera.projMat * stuff.camera.viewMat * light.modelMatrix;

        // Check if the camera is inside the light volume
        const bool inside = maths::point_in_sphere(stuff.camera.position, light.sphere, 1.05146f);

        // Add this point light to the main light passes
        pointPassVec.push_back({light.ubo, light.tex, matrix});
        pointShaftsVec.push_back({light.ubo, light.tex, matrix, !inside});
    }

    //--------------------------------------------------------//

    for (auto& [id, light] : dop::joined(mLightSpotSet, stuff.tables.spotlight))
    {
        // Calculate model-view-projection matrix for this light
        const Mat4F matrix = stuff.camera.projMat * stuff.camera.viewMat * light.modelMatrix;

        // Check if the camera is inside the light volume
        const bool inside = maths::point_in_cone(stuff.camera.position, light.cone, 1.00864f);

        // Add this spot light to the main light passes
        spotPassVec.push_back({light.ubo, light.tex, matrix});
        spotShaftsVec.push_back({light.ubo, light.tex, matrix, !inside});
    }
}

//============================================================================//

namespace { // anonymous

void add_simple_models_to_shadows ( data::DepthModelSimplePass& pass, const RenderStuff& stuff,
                                    const dop::Group& group, const Mat4F& lightMatrix )
{
    for (auto& [id, model] : dop::joined(group, stuff.tables.model))
    {
        const Mat4F matrix = lightMatrix * model.modelMatrix;

        if (model.hasMaskTexture == true)
        {
            for (uint index = 0u; index < model.materials.size(); ++index)
            {
//                const auto& mask = model.materials[index]->get_mask_handle();

//                // add this sub-mesh either to the punch part map, or the solid part vector
//                auto& dest = mask != nullptr ? pass.punchPartMap[&mask.get()] : pass.solidPartVec;
//                dest.push_back({model.mesh.get(), matrix, model.isMirrored, index});
            }
        }

        // no sub-meshes have a mask texture, so add to the full model vector
        else pass.solidFullVec.push_back({model.mesh.get(), matrix, model.isMirrored});
    }
}

void add_skelly_models_to_shadows ( data::DepthModelSkellyPass& pass, const RenderStuff& stuff,
                                    const dop::Group& group, const Mat4F& lightMatrix )
{
    for (auto& [id, model, skeleton] : dop::joined(group, stuff.tables.model, stuff.tables.skeleton))
    {
        const Mat4F matrix = lightMatrix * model.modelMatrix;

        if (model.hasMaskTexture == true)
        {
            for (uint index = 0u; index < model.materials.size(); ++index)
            {
//                const auto& mask = model.materials[index]->get_mask_handle();

//                // add this sub-mesh either to the punch part map, or the solid part vector
//                auto& dest = mask != nullptr ? pass.punchPartMap[&mask.get()] : pass.solidPartVec;
//                dest.push_back({skeleton.ubo, model.mesh.get(), matrix, model.isMirrored, index});
            }
        }

        // no sub-meshes have a mask texture, so add to the full model vector
        else pass.solidFullVec.push_back({skeleton.ubo, model.mesh.get(), matrix, model.isMirrored});
    }
}

} // anonymous

//============================================================================//

void PassesData::prepare_shadow_mapping(const RenderStuff& rstuff, const WorldStuff& wstuff)
{
    auto& cascPassVec = shadowsData.cascPassVec;
    auto& orthoPassVec = shadowsData.orthoPassVec;
    auto& pointPassVec = shadowsData.pointPassVec;
    auto& spotPassVec = shadowsData.spotPassVec;

    //--------------------------------------------------------//

    if (rstuff.skylight != nullptr)
    {
        const auto& light = *rstuff.skylight;
        const Vec2U viewPort = Vec2U(light.resolution);

        // iterate through individual cascades
        for (uint i = 0u; i < light.cascades; ++i)
        {
            // add the current cascade to the pass vector
            cascPassVec.push_back({light.fbos[i], viewPort, {}});

            // get the complete set of entities visible to this cascade
            const auto visibleSet = sys::system_get_visible_objects_ortho(wstuff, light.orthos[i]);

            // reduce the visible set down to sets for each type of entity
            const auto modelSimpleSet = dop::reduce(visibleSet, wstuff.groups.modelSimple);
            const auto modelSkellySet = dop::reduce(visibleSet, wstuff.groups.modelSkelly);

            add_simple_models_to_shadows ( cascPassVec.back().depthPasses.modelSimplePass,
                                           rstuff, modelSimpleSet, light.matrices[i] );

            add_skelly_models_to_shadows ( cascPassVec.back().depthPasses.modelSkellyPass,
                                           rstuff, modelSkellySet, light.matrices[i] );
        }
    }

    //--------------------------------------------------------//

    for (auto& [id, light] : dop::joined(mLightOrthoSet, rstuff.tables.ortholight))
    {
        const Vec2U viewport = Vec2U(light.resolution);

        orthoPassVec.push_back({light.fbo, viewport, {}});

        // get the complete set of entities visible to this light
        //dop::Group visibleSet = sys::system_get_visible_objects_frustum(wstuff, light.frustum);

        // reduce the visible set down to sets for each type of entity
        //auto modelSimpleSet = dop::reduce(visibleSet, mRenderSys->get_ModelSimpleSet());
        //auto modelSkellySet = dop::reduce(visibleSet, mRenderSys->get_ModelSkellySet());

        add_simple_models_to_shadows ( orthoPassVec.back().depthPasses.modelSimplePass,
                                       rstuff, wstuff.groups.modelSimple, light.lightMatrix );

        add_skelly_models_to_shadows ( orthoPassVec.back().depthPasses.modelSkellyPass,
                                       rstuff, wstuff.groups.modelSkelly, light.lightMatrix );
    }

    //--------------------------------------------------------//

    for (auto& [id, light] : dop::joined(mLightPointSet, rstuff.tables.pointlight))
    {
        const Vec2U viewport = Vec2U(light.resolution);

        for (uint face = 0u; face < 6u; ++face)
        {
            // add the current face to the pass vector
            pointPassVec.push_back({light.fbos[face], viewport, {}});

            // get the complete set of entities visible to this face
            const auto visibleSet = sys::system_get_visible_objects_frustum(wstuff, light.frustums[face]);

            // reduce the visible set down to sets for each type of entity
            const auto modelSimpleSet = dop::reduce(visibleSet, wstuff.groups.modelSimple);
            const auto modelSkellySet = dop::reduce(visibleSet, wstuff.groups.modelSkelly);

            add_simple_models_to_shadows ( pointPassVec.back().depthPasses.modelSimplePass,
                                           rstuff, modelSimpleSet, light.lightMatrices[face]);

            add_skelly_models_to_shadows ( pointPassVec.back().depthPasses.modelSkellyPass,
                                           rstuff, modelSkellySet, light.lightMatrices[face] );
        }
    }

    //--------------------------------------------------------//

    for (auto& [id, light] : dop::joined(mLightSpotSet, rstuff.tables.spotlight))
    {
        const Vec2U viewport = Vec2U(light.resolution);

        // add the current cascade to the pass vector
        spotPassVec.push_back({light.fbo, viewport, {}});

        // get the complete set of entities visible to this light
        const auto visibleSet = sys::system_get_visible_objects_frustum(wstuff, light.frustum);

        // reduce the visible set down to sets for each type of entity
        const auto modelSimpleSet = dop::reduce(visibleSet, wstuff.groups.modelSimple);
        const auto modelSkellySet = dop::reduce(visibleSet, wstuff.groups.modelSkelly);

        add_simple_models_to_shadows ( spotPassVec.back().depthPasses.modelSimplePass,
                                       rstuff, modelSimpleSet, light.lightMatrix );

        add_skelly_models_to_shadows ( spotPassVec.back().depthPasses.modelSkellyPass,
                                       rstuff, modelSkellySet, light.lightMatrix );
    }
}
