#pragma once

#include "setup.hpp"

namespace demo {

struct CameraBlock
{
    alignas(16) Mat4F viewMat;
    alignas(16) Mat4F projMat;
    alignas(16) Mat4F invViewMat;
    alignas(16) Mat4F invProjMat;
    alignas(16) Vec3F position;
    alignas(16) Vec3F direction;
};

struct LightBlock
{
    alignas(16) Vec3F ambiColour;
    alignas(16) Vec3F skyColour;
    alignas(16) Vec3F skyDirection;
    alignas(16) Mat4F skyMatrix;
};

struct StaticBlock
{
    alignas(16) Mat4F matrix;
    alignas(16) Mat34F normMat;
};

} // namespace demo
