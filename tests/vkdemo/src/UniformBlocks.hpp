#include <sqee/core/Types.hpp>

using namespace sq::coretypes;

namespace sqt {

struct CameraBlock
{
    Mat4F model;
    Mat4F view;
    Mat4F proj;
};

} // namespace sqt