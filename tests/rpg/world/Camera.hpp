#pragma once

#include <sqee/maths/Builtins.hpp>

namespace sqt::world {

//============================================================================//

class Camera final : sq::NonCopyable
{
public: //====================================================//

    /// Set the position of the camera.
    void set_position(Vec3F position);

    /// Set the direction of the camera.
    void set_direction(Vec3F direction);

    /// Set the vertical field of view.
    void set_fov(float fov);

    /// Set the viewport aspect ration.
    void set_aspect(float aspect);

    /// Set the far plane distance.
    void set_range(float range);

    //--------------------------------------------------------//

    /// Get the position of the camera.
    Vec3F get_position() const { return mPosition; }

    /// Get the direction of the camera.
    Vec3F get_direction() const { return mDirection; }

    /// Get the vertical field of view.
    float get_fov() const { return mFov; }

    /// Get the viewport aspect ration.
    float get_aspect() const { return mAspect; }

    /// Get the far plane distance.
    float get_range() const { return mRange; }

    //--------------------------------------------------------//

    /// Compute the camera view matrix.
    const Mat4F& get_view_matrix() const;

    /// Compute the camera proj matrix.
    const Mat4F& get_proj_matrix() const;

private: //===================================================//

    Vec3F mPosition  = { 0.f, 0.f, 4.f };
    Vec3F mDirection = { 0.f, 1.f, 0.f };
    float mFov       = 1.f;
    float mAspect    = 16.f / 9.f;
    float mRange     = 40.f;

    //--------------------------------------------------------//

    mutable Mat4F mViewMatrix = Mat4F();
    mutable Mat4F mProjMatrix = Mat4F();

    mutable bool mViewMatrixValid = false;
    mutable bool mProjMatrixValid = false;
};

//============================================================================//

} // namespace sqt::world
