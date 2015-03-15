#pragma once

#include "object.hpp"

namespace sqt {

class Light : public Object {
public:
    Light(const ObjSpec& _spec);

    vec3 pos, dir;
    vec3 colour;
    float angle, intensity, softness;
    uint texSize;
    mat4 shadMat;
};

}
