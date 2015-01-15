#pragma once

#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/models/mesh.hpp>

#include "object.hpp"

namespace sqt {

class Liquid : public Object {
public:
    Liquid(const ObjectSpec& _spec);

    float wScale;
    float wSmooth;
    glm::vec2 flow;
    glm::vec3 colour;
    float thickness;

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

    sq::Mesh mesh;
};

}
