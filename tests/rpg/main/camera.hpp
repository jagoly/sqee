#pragma once
#include <sqee/forward.hpp>

#include <sqee/render/cameras.hpp>

namespace sqt {

class MainCamera : public sq::UboCamera {
public:
    MainCamera();

    void set_pos(vec3 _pos);
    void set_dir(vec3 _dir);
};

}
