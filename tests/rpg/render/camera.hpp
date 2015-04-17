#pragma once
#include <sqee/forward.hpp>

#include <sqee/render/cameras.hpp>

namespace sqt {

class MainCamera : public sq::UboCamera {
public:
    MainCamera();

    array<pair<float, sq::Frustum>, 4> csmArr;

    void update();
    void recalc_frustums();
};

}
