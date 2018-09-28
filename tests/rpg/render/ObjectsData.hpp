#pragma once

#include <sqee/dop/Classes.hpp>

#include <sqee/maths/Volumes.hpp>

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>

#include <sqee/render/Armature.hpp>
#include <sqee/render/Material.hpp>
#include <sqee/render/Mesh.hpp>

#include "../systems/WorldStuff.hpp"

#include "../main/ResourceCaches.hpp"
#include "../main/Options.hpp"

//============================================================================//

namespace sqt {

class RenderStuff final : sq::NonCopyable
{
public: //====================================================//

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
        String path;
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

        std::array<sq::maths::Sphere, 6> spheres;
        std::array<sq::maths::Ortho2D, 6> orthos;
        std::array<Mat4F, 6> matrices;

        sq::UniformBuffer ubo;
        sq::TextureArray2D tex;

        uint cascades;
    };

    //--------------------------------------------------------//

    struct ModelData
    {
        std::vector<MaterialHandle> materials;
        MeshHandle mesh;

        Mat4F modelMatrix;
        bool hasMaskTexture;
        bool isMirrored;
    };

    //--------------------------------------------------------//

    struct SkeletonData
    {
        ArmatureHandle armature;

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

        sq::maths::Sphere sphere;

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

        sq::maths::Cone cone;

        sq::UniformBuffer ubo;
        sq::Texture2D tex;

        Mat4F modelMatrix;
    };

    //--------------------------------------------------------//

    CameraData camera;

    UniquePtr<SkyboxData> skybox;
    UniquePtr<AmbientData> ambient;
    UniquePtr<SkylightData> skylight;

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
