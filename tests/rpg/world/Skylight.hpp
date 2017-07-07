#pragma once

#include <sqee/builtins.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>
#include <sqee/maths/Matrices.hpp>

#include "Camera.hpp"

namespace sqt::world {

//============================================================================//

class Skylight final : sq::NonCopyable
{
public: //====================================================//

    /// Constructor.
    Skylight(const Camera& camera) : camera(camera) {}

    //--------------------------------------------------------//

    /// Set the rotation from basis.
    void set_rotation(QuatF rotation);

    /// Set the colour of the light.
    void set_colour(Vec3F colour);

    /// Set the volumetric shaft density.
    void set_density(float density);

    /// Set the base shadow map resolution.
    void set_resolution(uint resolution);

    /// Set the number of cascades.
    void set_cascades(uint cascades);

    //--------------------------------------------------------//

    /// Get the rotation from basis.
    QuatF get_rotation() const { return mRotation; }

    /// Get the colour of the light.
    Vec3F get_colour() const { return mColour; }

    /// Get the volumetric shaft density.
    float get_density() const { return mDensity; }

    /// Get the base shadow map resolution.
    uint get_resolution() const { return mResolution; }

    /// Get the number of cascades.
    uint get_cascades() const { return mCascades; }

    //--------------------------------------------------------//

    /// Compute the light matrices.
    std::array<Mat4F, 6> get_matrices() const;

private: //===================================================//

    QuatF mRotation   = { 0.f, 0.f, 0.f, 1.f };
    Vec3F mColour     = { 1.f, 1.f, 1.f };
    float mDensity    = 0.f;
    uint  mResolution = 1024u;
    uint  mCascades   = 4u;

    //--------------------------------------------------------//

    const Camera& camera;
};

//============================================================================//

} // namespace sqt::world
