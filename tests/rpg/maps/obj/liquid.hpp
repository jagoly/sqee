#pragma once

#include <sqee/gl/uniformbuffers.hpp>

#include "object.hpp"

namespace sqt {
namespace wld {

class Liquid : public Object {
public:
    Liquid(const ObjectSpec& _spec);

    float smooth = 0.f;
    float scale = 1.f;
    glm::vec2 flow = {0.f, 0.f};
    glm::vec3 colour = {0.f, 0.f, 0.f};
    float thickness = 1.f;

    GLuint vao;
    GLuint vboP, vboTc;
    sq::Texture* normArray;

    sq::UniformBuffer ubo;

    void update_ubo();
    void bind_ubo();
    void draw();

    void tick();
    void calc(double _accum);

private:
    glm::vec3 normInfo;
    glm::vec2 flowOffs;
    double normOffsA, normOffsB;
    glm::vec2 flowOffsA, flowOffsB;
};

}
}
