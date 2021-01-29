#include <sqee/core/Types.hpp>

using namespace sq::coretypes;

namespace sqt {

struct CameraBlock
{
    Mat4F view;
    Mat4F proj;
};

struct ModelBlock
{
    Mat4F model;
};

} // namespace sqt
