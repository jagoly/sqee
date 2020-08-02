#include <sqee/maths/Functions.hpp>

#include "Skylight.hpp"

using namespace sqt::world;
namespace maths = sq::maths;

//============================================================================//

void Skylight::set_rotation(QuatF rotation) { mRotation = rotation; }

void Skylight::set_colour(Vec3F colour) { mColour = colour; }

void Skylight::set_density(float density) { mDensity = density; }

void Skylight::set_resolution(uint resolution) { mResolution = resolution; }

void Skylight::set_cascades(uint cascades) { mCascades = cascades; }

//============================================================================//

void Skylight::compute_cascades ( std::array<sq::maths::Sphere, 6>& spheres,
                                  std::array<sq::maths::Ortho2D, 6>& orthos,
                                  std::array<Mat4F, 6>& matrices ) const
{
    const Vec3F cameraPos = camera.get_position();
    const Vec3F cameraDir = camera.get_direction();
    const float cameraRange = camera.get_range();

    const Mat4F cameraMatrix = camera.get_proj_matrix() * camera.get_view_matrix();
    const Mat4F invCameraMatrix = maths::inverse(cameraMatrix);

    //--------------------------------------------------------//

    const Vec3F centreNear = cameraPos + cameraDir * 0.1f;
    const Vec3F centreFar = cameraPos + cameraDir * (cameraRange - 0.1f);

    constexpr auto divide_by_w = [](Vec4F v) { return Vec3F(v) / v.w; };

    const Vec3F cornerNear = divide_by_w(invCameraMatrix * Vec4F(1.f, 1.f, 0.f, 1.f));
    const Vec3F cornerFar = divide_by_w(invCameraMatrix * Vec4F(1.f, 1.f, 1.f, 1.f));

    const float splitSize = 1.f / float(mCascades);

    const Mat3F basis = Mat3F(mRotation);

    //--------------------------------------------------------//

    for (uint i = 0u; i < mCascades; ++i)
    {
        const float splitB = splitSize * float(i + 1u);

        const Vec3F centreF = maths::mix(centreNear, centreFar, splitB);
        const Vec3F cornerF = maths::mix(cornerNear, cornerFar, splitB);

        spheres[i].origin = centreF;
        spheres[i].radius = maths::distance(centreF, cornerF);

        //--------------------------------------------------------//

        orthos[i].normalX = basis[0];
        orthos[i].normalY = basis[1];

        const Vec3F sideOffsetX = orthos[i].normalX * spheres[i].radius;
        const Vec3F sideOffsetY = orthos[i].normalY * spheres[i].radius;
        const Vec3F sideOffsetZ = basis[2] * spheres[i].radius;

        orthos[i].offsetsX[0] = maths::dot(-orthos[i].normalX, spheres[i].origin + sideOffsetX);
        orthos[i].offsetsX[1] = maths::dot(+orthos[i].normalX, spheres[i].origin - sideOffsetX);
        orthos[i].offsetsY[0] = maths::dot(-orthos[i].normalY, spheres[i].origin + sideOffsetY);
        orthos[i].offsetsY[1] = maths::dot(+orthos[i].normalY, spheres[i].origin - sideOffsetY);

        orthos[i].planeZ = { -basis[2], maths::dot(basis[2], spheres[i].origin - sideOffsetZ) };

        //--------------------------------------------------------//

        Mat4F viewMat = maths::look_at_RH(spheres[i].origin, spheres[i].origin - basis[2], basis[1]);

        viewMat[3].x -= std::fmod(viewMat[3].x, 2.f * spheres[i].radius / float(mResolution));
        viewMat[3].y -= std::fmod(viewMat[3].y, 2.f * spheres[i].radius / float(mResolution));
        viewMat[3].z -= std::fmod(viewMat[3].z, 2.f * spheres[i].radius / float(mResolution));

        Mat4F projMat = maths::ortho_RH(Vec3F(-spheres[i].radius), Vec3F(+spheres[i].radius));

        matrices[i] = projMat * viewMat;
    }
}
