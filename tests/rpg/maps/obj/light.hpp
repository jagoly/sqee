#pragma once

#include <sqee/gl/framebuffers.hpp>

#include "object.hpp"

namespace sqt {
namespace wld {

class Light : public Object {
public:
    Light(const ObjectSpec& _spec);

    glm::vec3 pos, dir;
    glm::vec3 colour;
    float angle, intensity, softness;
    uint texSize;
    glm::mat4 shadMat;
};

}
}
