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

std::array<Mat4F, 6> Skylight::get_matrices() const
{
    std::array<Mat4F, 6> result;

    const float splitSize = camera.get_range() / float(mCascades);

    const Vec3F cameraPos = camera.get_position();
    const Vec3F cameraDir = camera.get_direction();

    for (uint i = 0u; i < mCascades; ++i)
    {
        const float splitA = splitSize * float(i);
        const float splitB = splitA + splitSize;

        const Vec3F centre = cameraPos + cameraDir * ((splitA + splitB) / 2.f);

        Mat4F viewMat = Mat4F(maths::conjugate(mRotation));
        viewMat = maths::translate(viewMat, -centre);

        viewMat[3].x -= std::fmod(viewMat[3].x, 2.f * splitB / float(mResolution));
        viewMat[3].y -= std::fmod(viewMat[3].y, 2.f * splitB / float(mResolution));
        viewMat[3].z -= std::fmod(viewMat[3].z, 2.f * splitB / float(mResolution));

        Mat4F projMat = maths::ortho(Vec3F(-splitB), Vec3F(+splitB));

        result[i] = projMat * viewMat;
    }

    return result;
}
