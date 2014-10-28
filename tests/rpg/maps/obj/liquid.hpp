#pragma once

#include "object.hpp"

namespace sqt {
namespace obj {

class Liquid : public Object {
public:
    using Object::Object;
    void create();

    glm::vec2 flowOffsetA, flowOffsetB;
    glm::vec3 tinge;
    float scale;
    float wSmooth;

    GLuint vboP, vboTc;
    GLuint vao;

    glm::mat4 reflMat;
    float zPos;

    void tick();
private:
    float xPT, yPT;
};

}
}
