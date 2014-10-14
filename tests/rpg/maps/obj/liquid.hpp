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
    GLfloat scale;
    GLfloat wSmooth;
    GLuint vao;
    void tick(int _tickRate);
private:
    float xPT, yPT;
};

}
}
