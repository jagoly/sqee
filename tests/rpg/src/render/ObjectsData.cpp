#include <sqee/core/Utilities.hpp>

#include <sqee/redist/gl_loader.hpp>

#include <sqee/maths/Functions.hpp>
#include <sqee/misc/DopFunctions.hpp>

#include <sqee/objects/Mesh.hpp>
#include <sqee/objects/Material.hpp>
#include <sqee/objects/Armature.hpp>

#include "../main/Options.hpp"
#include "../systems/WorldStuff.hpp"

#include "ObjectsData.hpp"

//============================================================================//

namespace dop = sq::dop;
namespace maths = sq::maths;

using namespace sqt;

//============================================================================//

RenderStuff::RenderStuff()
{
    camera.ubo.create_and_allocate(352u);
}

//============================================================================//

void sqt::refresh_render_stuff(RenderStuff& rstuff, const WorldStuff& wstuff, const Options& options)
{
    const auto& wTables = wstuff.tables;
    auto& rTables = rstuff.tables;

    //--------------------------------------------------------//

    for (const auto& id : wstuff.deletedEntities.mIds) rTables.model.try_erase(id);
    for (const auto& id : wstuff.deletedEntities.mIds) rTables.skeleton.try_erase(id);
    for (const auto& id : wstuff.deletedEntities.mIds) rTables.ortholight.try_erase(id);
    for (const auto& id : wstuff.deletedEntities.mIds) rTables.pointlight.try_erase(id);
    for (const auto& id : wstuff.deletedEntities.mIds) rTables.spotlight.try_erase(id);

    //========================================================//
    // update camera data                                     //
    //========================================================//

    RenderStuff::CameraData& camera = rstuff.camera;

    camera.position = wstuff.camera.get_position();
    camera.direction = wstuff.camera.get_direction();
    camera.range = wstuff.camera.get_range();

    camera.viewMat = wstuff.camera.get_view_matrix();
    camera.projMat = wstuff.camera.get_proj_matrix();

    //--------------------------------------------------------//

    camera.frustum = maths::make_frustum ( camera.projMat * camera.viewMat,
                                           camera.position, camera.direction,
                                           camera.range );

    camera.ubo.update ( 0u, sq::Structure ( camera.viewMat, camera.projMat,
                        maths::inverse(camera.viewMat), maths::inverse(camera.projMat),
                        maths::transpose(camera.viewMat),
                        camera.position, 0, camera.direction, 0 ) );

    //========================================================//
    // update skybox data                                     //
    //========================================================//

    if (wstuff.skybox == nullptr)
        rstuff.skybox.reset();

    //--------------------------------------------------------//

    else if (rstuff.skybox == nullptr)
    {
        rstuff.skybox = std::make_unique<RenderStuff::SkyboxData>();

        rstuff.skybox->tex.set_format(sq::Texture::Format::RGB8_UN);
        rstuff.skybox->tex.set_filter_mode(true);
    }

    //--------------------------------------------------------//

    if (auto skybox = rstuff.skybox.get())
    {
        auto& newPath = wstuff.skybox->get_texture();

        if (skybox->path != newPath)
        {
            skybox->tex.load_automatic("assets/textures/" + newPath);
            skybox->path = newPath;
        }

        skybox->params.r = wstuff.skybox->get_saturation();
        skybox->params.g = wstuff.skybox->get_brightness();
        skybox->params.b = wstuff.skybox->get_contrast();
        skybox->params.a = wstuff.skybox->get_opacity();
    }

    //========================================================//
    // update ambient data                                    //
    //========================================================//

    if (wstuff.ambient == nullptr)
        rstuff.ambient.reset();

    //--------------------------------------------------------//

    else if (rstuff.ambient == nullptr)
    {
        rstuff.ambient = std::make_unique<RenderStuff::AmbientData>();
    }

    //--------------------------------------------------------//

    if (auto ambient = rstuff.ambient.get())
    {
        ambient->colour = wstuff.ambient->get_colour();
    }

    //========================================================//
    // update skylight data                                   //
    //========================================================//

    if (wstuff.skylight == nullptr)
        rstuff.skylight.reset();

    //--------------------------------------------------------//

    else if (rstuff.skylight == nullptr)
    {
        rstuff.skylight = std::make_unique<RenderStuff::SkylightData>();

        rstuff.skylight->tex.set_format(sq::Texture::Format::DEPTH16);
        rstuff.skylight->tex.set_filter_mode(true);
        rstuff.skylight->tex.set_shadow_mode(true);

        rstuff.skylight->ubo.create_and_allocate(512u);
    }

    //--------------------------------------------------------//

    if (auto skylight = rstuff.skylight.get())
    {
        skylight->cascades = wstuff.skylight->get_cascades();

        wstuff.skylight->compute_cascades(skylight->spheres, skylight->orthos, skylight->matrices);

        const uint resolution = wstuff.skylight->get_resolution() * (options.Shadows_Large + 1u);
        const Vec3U newSize { resolution, resolution, skylight->cascades };

        if (skylight->tex.get_size() != newSize)
        {
            skylight->tex.allocate_storage(newSize);

            for (uint i = 0u; i < skylight->cascades; ++i)
                skylight->fbos[i].attach(gl::DEPTH_ATTACHMENT, skylight->tex, i);
        }

        skylight->ubo.update ( 0u, sq::Structure ( wstuff.skylight->get_rotation() * Vec3F(0, 0, -1), skylight->cascades,
                               wstuff.skylight->get_colour(), camera.range / float(skylight->cascades),
                               skylight->spheres, skylight->matrices ) );
    }

    //========================================================//
    // update model table                                     //
    //========================================================//

    rstuff.tables.model.clear();

    for (auto& [id, transform, model] : dop::joined(wTables.transform, wTables.model))
    {
        rstuff.tables.model.insert(id, {});
        auto& entry = *rstuff.tables.model.back();

        entry.materials = model.materials;
        entry.materials.shrink_to_fit();

        entry.hasMaskTexture = false;

        for (const auto& material : entry.materials)
            entry.hasMaskTexture |= material->has_mask_texture();

        entry.mesh = model.mesh;

        entry.modelMatrix = model.matrix;
        entry.isMirrored = std::signbit(maths::determinant(entry.modelMatrix));
    }

    //========================================================//
    // update skeleton table                                  //
    //========================================================//

    rstuff.tables.skeleton.clear();

    for (auto& [id, skeleton] : dop::joined(wTables.skeleton))
    {
        rstuff.tables.skeleton.insert(id, {});
        auto& entry = *rstuff.tables.skeleton.back();

        entry.ubo.create_and_allocate(3840u);

        entry.armature = skeleton.armature;

//        const auto& pose = data.armature->poseMap.at("Dead");
//        auto uboData = sq::Armature::make_UboData(pose);
//        data.ubo.update("bones", uboData.data());
    }

    //========================================================//
    // update ortholight table                                //
    //========================================================//

    for (const auto& id : wTables.ortholight.mIds)
    {
        if (rTables.ortholight.exists(id) == false)
        {
            rTables.ortholight.insert(id, {});
            auto& entry = *rTables.ortholight.back();

            entry.tex.set_format(sq::Texture::Format::DEPTH16);
            entry.tex.set_filter_mode(true);
            entry.tex.set_shadow_mode(true);

            entry.ubo.create_and_allocate(96u);
        }
    }

    //--------------------------------------------------------//

    for (auto& [id, transform, light, entry] : dop::joined(wTables.transform, wTables.ortholight, rTables.ortholight))
    {
        const uint newSize = light.resolution * (options.Shadows_Large + 1u);

        if (entry.tex.get_size().x != newSize)
        {
            entry.tex.allocate_storage(Vec2U(newSize));
            entry.fbo.attach(gl::DEPTH_ATTACHMENT, entry.tex);
        }

        // calculate direction and tangent vectors
        Vec3F direction = maths::normalize(Mat3F(transform.worldMatrix) * Vec3F(0.f, 0.f, -1.f));
        Vec3F tangent = maths::normalize(Mat3F(transform.worldMatrix) * Vec3F(0.f, +1.f, 0.f));

        // calculate view and projection matrices
        Mat4F viewMat = maths::look_at_RH(transform.worldPosition, transform.worldPosition + direction, tangent);
        Mat4F projMat = maths::ortho_RH(light.minimum * transform.worldScale, light.maximum * transform.worldScale);

        entry.lightMatrix = projMat * viewMat;

        // calculate ortho model matrix
        entry.modelMatrix = maths::inverse(entry.lightMatrix);

        // TODO: calculate bounding volume

        entry.ubo.update ( 0u, sq::Structure ( direction, light.density, light.colour, 0, entry.lightMatrix ) );
    }

    //========================================================//
    // update pointlight table                                //
    //========================================================//

    for (const auto& id : wTables.pointlight.mIds)
    {
        if (rTables.pointlight.exists(id) == false)
        {
            rTables.pointlight.insert(id, {});
            auto& entry = *rTables.pointlight.back();

            entry.tex.set_format(sq::Texture::Format::DEPTH16);
            entry.tex.set_filter_mode(true);
            entry.tex.set_shadow_mode(true);

            entry.ubo.create_and_allocate(416u);
        }
    }

    //--------------------------------------------------------//

    for (auto& [id, transform, light, entry] : dop::joined(wTables.transform, wTables.pointlight, rTables.pointlight))
    {
        const uint newSize = light.resolution * (options.Shadows_Large + 1u);

        if (entry.tex.get_size().x != newSize)
        {
            entry.tex.allocate_storage(uint(newSize));

            for (uint face = 0u; face < 6u; ++face)
                entry.fbos[face].attach(gl::DEPTH_ATTACHMENT, entry.tex, face);
        }

        const Vec3F& position = transform.worldPosition;
        const float& scale = transform.worldScale;

        entry.modelMatrix = maths::scale(maths::translate(Mat4F(), position), Vec3F(scale));

        entry.sphere = maths::Sphere { position, scale };

        const Mat4F projMat = maths::perspective_RH(maths::radians(0.25f), 1.f, 0.1f, scale);

        entry.lightMatrices[0] = projMat * maths::look_at_RH(position, position + Vec3F(+1, 0, 0), Vec3F(0, -1, 0));
        entry.lightMatrices[1] = projMat * maths::look_at_RH(position, position + Vec3F(-1, 0, 0), Vec3F(0, -1, 0));
        entry.lightMatrices[2] = projMat * maths::look_at_RH(position, position + Vec3F(0, +1, 0), Vec3F(0, 0, +1));
        entry.lightMatrices[3] = projMat * maths::look_at_RH(position, position + Vec3F(0, -1, 0), Vec3F(0, 0, -1));
        entry.lightMatrices[4] = projMat * maths::look_at_RH(position, position + Vec3F(0, 0, +1), Vec3F(0, -1, 0));
        entry.lightMatrices[5] = projMat * maths::look_at_RH(position, position + Vec3F(0, 0, -1), Vec3F(0, -1, 0));

        entry.frustums[0] = maths::make_frustum(entry.lightMatrices[0], position, Vec3F(+1, 0, 0), scale);
        entry.frustums[1] = maths::make_frustum(entry.lightMatrices[1], position, Vec3F(-1, 0, 0), scale);
        entry.frustums[2] = maths::make_frustum(entry.lightMatrices[2], position, Vec3F(0, +1, 0), scale);
        entry.frustums[3] = maths::make_frustum(entry.lightMatrices[3], position, Vec3F(0, -1, 0), scale);
        entry.frustums[4] = maths::make_frustum(entry.lightMatrices[4], position, Vec3F(0, 0, +1), scale);
        entry.frustums[5] = maths::make_frustum(entry.lightMatrices[5], position, Vec3F(0, 0, -1), scale);

        entry.ubo.update ( 0u, sq::Structure ( position, scale, light.colour, 0, entry.lightMatrices ) );
    }

    //========================================================//
    // update spotlight table                                 //
    //========================================================//

    for (const auto& id : wTables.spotlight.mIds)
    {
        if (rTables.spotlight.exists(id) == false)
        {
            rTables.spotlight.insert(id, {});
            auto& entry = *rTables.spotlight.back();

            entry.tex.set_format(sq::Texture::Format::DEPTH16);
            entry.tex.set_filter_mode(true);
            entry.tex.set_shadow_mode(true);

            entry.ubo.create_and_allocate(112u);
        }
    }

    //--------------------------------------------------------//

    for (auto& [id, transform, light, entry] : dop::joined(wTables.transform, wTables.spotlight, rTables.spotlight))
    {
        const uint newSize = light.resolution * (options.Shadows_Large + 1u);

        if (entry.tex.get_size().x != newSize)
        {
            entry.tex.allocate_storage(Vec2U(newSize));
            entry.fbo.attach(gl::DEPTH_ATTACHMENT, entry.tex);
        }

        const float angle = maths::radians(light.angle);

        // calculate direction and tangent vectors
        Vec3F direction = maths::normalize(Mat3F(transform.worldMatrix) * Vec3F(0.f, 0.f, -1.f));
        Vec3F tangent = maths::normalize(Mat3F(transform.worldMatrix) * Vec3F(0.f, 1.f, 0.f));

        // calculate view and projection matrices
        Mat4F viewMat = maths::look_at_RH(transform.worldPosition, transform.worldPosition + direction, tangent);
        Mat4F projMat = maths::perspective_RH(angle * 2.f, 1.f, 0.2f, transform.worldScale);

        entry.lightMatrix = projMat * viewMat;

        // calculate light bounding frustum
        entry.frustum = sq::maths::make_frustum ( entry.lightMatrix, transform.worldPosition,
                                                  direction, transform.worldScale );

        entry.cone = { transform.worldPosition, transform.worldScale, direction, std::tan(angle) * transform.worldScale };

        // calculate cone volume size and model matrix
        Vec3F modelScale = Vec3F(Vec2F(std::tan(angle)), 1.f) * transform.worldScale;
        entry.modelMatrix = maths::scale(maths::inverse(viewMat), modelScale);

        entry.ubo.update ( 0u, sq::Structure ( transform.worldPosition, transform.worldScale,
                           direction, light.softness, light.colour, angle, entry.lightMatrix ) );
    }
}
