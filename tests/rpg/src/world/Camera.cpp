#include <sqee/maths/Functions.hpp>

#include "Camera.hpp"

namespace maths = sq::maths;
using namespace sqt::world;

//============================================================================//

void Camera::set_position(Vec3F position) { mPosition = position; mViewMatrixValid = false; }

void Camera::set_direction(Vec3F direction) { mDirection = direction; mViewMatrixValid = false; }

//============================================================================//

void Camera::set_fov(float fov) { mFov = fov; mProjMatrixValid = false; }

void Camera::set_aspect(float aspect) { mAspect = aspect; mProjMatrixValid = false; }

void Camera::set_range(float range) { mRange = range; mProjMatrixValid = false; }

//============================================================================//

const Mat4F& Camera::get_view_matrix() const
{
    if (mViewMatrixValid == false)
    {
        mViewMatrix = maths::look_at_RH(mPosition, mPosition + mDirection, {0.f, 0.f, 1.f});

        mViewMatrixValid = true;
    }

    return mViewMatrix;
}

const Mat4F& Camera::get_proj_matrix() const
{
    if (mProjMatrixValid == false)
    {
        mProjMatrix = maths::perspective_RH(mFov, mAspect, 0.1f, mRange);

        mProjMatrixValid = true;
    }

    return mProjMatrix;
}
