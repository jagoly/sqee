#pragma once

#include "object.hpp"

namespace sqt {
namespace obj {

class Liquid : public Object {
public:
    typedef std::shared_ptr<Liquid> Ptr;
    using Object::Object;
    void create();

    glm::vec2 flowOffsetA, flowOffsetB;
    glm::vec3 tinge;
    GLfloat zPos;
    GLfloat wScale;
    GLfloat wSmooth;
    GLuint vao;
    void tick(int _tickRate);
private:
    float xPT, yPT;
};

}
}
