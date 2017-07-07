#pragma once

#include <sqee/dop/Classes.hpp>

#include <sqee/misc/ResourceHandle.hpp>

#include <sqee/maths/Volumes.hpp>

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>

#include <sqee/render/Armature.hpp>
#include <sqee/render/Material.hpp>
#include <sqee/render/Mesh.hpp>

#include "../Options.hpp"

namespace sqt {

//============================================================================//

class WorldStuff; // Forward Declaration

//============================================================================//

struct RenderStuff : sq::NonCopyable
{
    RenderStuff();

    //--------------------------------------------------------//

    struct CameraData
    {
        Vec3F position;
        Vec3F direction;
        float range;

        Mat4F viewMat;
        Mat4F projMat;

        sq::maths::Frustum frustum;
        sq::UniformBuffer ubo;
    };

    //--------------------------------------------------------//

    struct SkyboxData
    {
        string path;
        sq::TextureCube tex;
        Vec4F params;
    };

    //--------------------------------------------------------//

    struct AmbientData
    {
        Vec3F colour;
    };

    //--------------------------------------------------------//

    struct SkylightData
    {
        std::array<sq::FrameBuffer, 6> fbos;

        std::array<Mat4F, 6> matrices;
        std::array<sq::maths::Planes<4>, 6> planes;

        sq::UniformBuffer ubo;
        sq::TextureArray2D tex;

        uint cascades;
    };

    //--------------------------------------------------------//

    struct ModelData
    {
        std::vector<sq::Handle<sq::Material>> materials;
        sq::Handle<sq::Mesh> mesh;

        Mat4F modelMatrix;
        bool hasMaskTexture;
        bool isMirrored;
    };

    //--------------------------------------------------------//

    struct SkeletonData
    {
        sq::Handle<sq::Armature> armature;

        sq::UniformBuffer ubo;
    };

    //--------------------------------------------------------//

    struct OrthoLightData
    {
        sq::FrameBuffer fbo;
        Mat4F lightMatrix;

        sq::UniformBuffer ubo;
        sq::Texture2D tex;

        Mat4F modelMatrix;
    };

    //--------------------------------------------------------//

    struct PointLightData
    {
        std::array<sq::FrameBuffer, 6> fbos;

        std::array<Mat4F, 6> lightMatrices;
        std::array<sq::maths::Frustum, 6> frustums;

        sq::UniformBuffer ubo;
        sq::TextureCube tex;

        Mat4F modelMatrix;
    };

    //--------------------------------------------------------//

    struct SpotLightData
    {
        sq::FrameBuffer fbo;

        Mat4F lightMatrix;
        sq::maths::Frustum frustum;

        sq::UniformBuffer ubo;
        sq::Texture2D tex;

        Mat4F modelMatrix;
    };

    //--------------------------------------------------------//

    CameraData camera;

    unique_ptr<SkyboxData> skybox;
    unique_ptr<AmbientData> ambient;
    unique_ptr<SkylightData> skylight;

    //--------------------------------------------------------//

    struct {
        sq::dop::Table<ModelData>      model;
        sq::dop::Table<SkeletonData>   skeleton;
        sq::dop::Table<OrthoLightData> ortholight;
        sq::dop::Table<PointLightData> pointlight;
        sq::dop::Table<SpotLightData>  spotlight;
    } tables;
};

//============================================================================//

void refresh_render_stuff(RenderStuff& rstuff, const WorldStuff& wstuff, const Options& options);

//============================================================================//

} // namespace sqt
