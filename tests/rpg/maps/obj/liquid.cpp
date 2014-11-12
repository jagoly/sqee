#include "liquid.hpp"

using namespace sqt::wld;

void Liquid::create(ObjectSpec& _spec) {
    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);
    gl::EnableVertexAttribArray(0);
    gl::EnableVertexAttribArray(1);

    float points[18];
    float txcrds[12];

    if (_spec.floatMap.count("pos")) {
        pos = glm::make_vec3(_spec.floatMap["pos"].data());
    }

    if (_spec.floatMap.count("size"))
        size = glm::make_vec2(_spec.floatMap["size"].data());

    if (_spec.floatMap.count("scale"))
        scale = _spec.floatMap["scale"][0];

    if (_spec.floatMap.count("smooth"))
        smooth = _spec.floatMap["smooth"][0];

    if (_spec.floatMap.count("flow"))
        flow = glm::make_vec2(_spec.floatMap["flow"].data());

    if (_spec.floatMap.count("colour"))
        colour = glm::make_vec4(_spec.floatMap["colour"].data());

    float tX = size.x / scale;
    float tY = size.y / scale;

    points[5*3+0] = points[1*3+0] = size.x;
    txcrds[5*2+0] = txcrds[1*2+0] = tX;
    points[5*3+1] = points[1*3+1] = size.y;
    txcrds[5*2+1] = txcrds[1*2+1] = tY;

    points[4*3+0] = points[2*3+0] = 0.f;
    txcrds[4*2+0] = txcrds[2*2+0] = 0.f;
    points[4*3+1] = points[2*3+1] = 0.f;
    txcrds[4*2+1] = txcrds[2*2+1] = 0.f;

    points[3*3+0] = 0.f;
    txcrds[3*2+0] = 0.f;
    points[3*3+1] = size.y;
    txcrds[3*2+1] = tY;

    points[0*3+0] = size.x;
    txcrds[0*2+0] = tX;
    points[0*3+1] = 0.f;
    txcrds[0*2+1] = 0.f;

    points[2] = points[5] = points[8] = points[11] = points[14] = points[17] = 0.f;

    gl::GenBuffers(1, &vboP);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboP);
    gl::BufferData(gl::ARRAY_BUFFER, 18 * sizeof(float), points, gl::STATIC_DRAW);
    gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 0, nullptr);

    gl::GenBuffers(1, &vboTc);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboTc);
    gl::BufferData(gl::ARRAY_BUFFER, 12 * sizeof(float), txcrds, gl::STATIC_DRAW);
    gl::VertexAttribPointer(1, 2, gl::FLOAT, false, 0, nullptr);

    reflMat = {1, 0, 0, 0,
               0, 1, 0, 0,
               0, 0, -1, 0,
               0, 0, 2.f*pos.z, 1};
}

void Liquid::tick() {
    flowOffsA = flowOffsB;
    flowOffsB.x += flow.x / scale;
    flowOffsB.y += flow.y / scale;
}

void Liquid::calc(double _accum) {
    const double dt = 1.d / 24.d;
    flowOffs = glm::mix(flowOffsA, flowOffsB, _accum / dt);
}
