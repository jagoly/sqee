#pragma once

#include "object.hpp"

namespace sqt {
namespace wld {

class Liquid : public Object {
public:
    using Object::Object;
    void create(ObjectSpec& _spec);

    glm::vec3 pos = {0.f, 0.f, 0.f};
    glm::vec2 size = {1.f, 1.f};
    float scale = 1.f;
    float smooth = 0.f;
    glm::vec2 flow = {0.f, 0.f};
    glm::vec4 colour = {0.f, 0.f, 0.f, 1.f};

    GLuint vao;
    GLuint vboP, vboTc;

    glm::vec2 flowOffs;
    glm::mat4 reflMat;

    void tick();
    void calc(double _accum);

private:
    float fX, fY;
    glm::vec2 flowOffsA, flowOffsB;
};

}
}
