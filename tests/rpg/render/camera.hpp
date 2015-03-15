#pragma once
#include <sqee/forward.hpp>

#include <sqee/gl/cameras.hpp>
#include <sqee/gl/maths.hpp>

namespace sqt {

class MainCamera : public sq::UboCamera {
public:
    MainCamera();

    array<std::pair<float, sq::Frustum>, 4> csmArr;

    void update();
    void recalc_frustums();
};

}
