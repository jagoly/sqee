#pragma once

#include <sqee/dop/Classes.hpp>

#include <sqee/misc/Resource.hpp>

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>

#include <sqee/render/Armature.hpp>
#include <sqee/render/Material.hpp>
#include <sqee/render/Mesh.hpp>

#include "world/Camera.hpp"
#include "world/SkyBox.hpp"
#include "world/Ambient.hpp"
#include "world/SkyLight.hpp"

#include "../helpers.hpp"

namespace sqt { namespace render {

//============================================================================//

struct ObjectsData : sq::NonCopyable
{
    //========================================================//

    struct RenderModelData
    {
        vector<sq::Handle<sq::Material>> materials;
        sq::Handle<sq::Mesh> mesh;

        Mat4F modelMatrix;
        bool hasMaskTexture;
        bool isMirrored;
    };

    //========================================================//

    struct RenderSkeletonData
    {
        sq::Handle<sq::Armature> armature;

        sq::UniformBuffer ubo;
    };

    //========================================================//

    struct RenderLightCascData
    {
        array<int32_t, 6> cullFrusIds;

        array<sq::FrameBuffer, 6> fbos;
        array<Mat4F, 6> lightMatArr;

        sq::UniformBuffer ubo;
        sq::TextureArray2D tex;

        uint cascadeCount;
        float cascadeSize;
    };

    //========================================================//

    struct RenderLightPointData
    {
        array<sq::FrameBuffer, 6> fbos;
        array<Mat4F, 6> lightMatArr;

        sq::UniformBuffer ubo;
        sq::TextureCube tex;

        Mat4F modelMatrix;
    };

    //========================================================//

    struct RenderLightSpotData
    {
        sq::FrameBuffer fbo;
        Mat4F lightMatrix;

        sq::UniformBuffer ubo;
        sq::Texture2D tex;

        Mat4F modelMatrix;
    };

    //========================================================//

    sq::dop::Table<RenderModelData> mModelTable;
    sq::dop::Table<RenderSkeletonData> mSkeletonTable;
    sq::dop::Table<RenderLightSpotData> mLightSpotTable;

    unique_ptr<render::CameraData> cameraData;
    unique_ptr<render::SkyBoxData> skyboxData;
    unique_ptr<render::AmbientData> ambientData;
    unique_ptr<render::SkyLightData> skylightData;
};

//============================================================================//

void refresh_render_tables(ObjectsData& tables);

//============================================================================//

}} // namespace sqt::render
