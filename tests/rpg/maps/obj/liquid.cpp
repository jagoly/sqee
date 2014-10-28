#include <iostream>

#include "liquid.hpp"

using namespace sqt::obj;

void Liquid::create() {
#ifdef SQEE_DEBUG
    SQ_FLOATCHECK("xPos")
    SQ_FLOATCHECK("yPos")
    SQ_FLOATCHECK("xSize")
    SQ_FLOATCHECK("ySize")
    SQ_FLOATCHECK("zPos")
    SQ_FLOATCHECK("xFlow")
    SQ_FLOATCHECK("yFlow")
    SQ_FLOATCHECK("speed")
    SQ_FLOATCHECK("wSmooth")
    SQ_FLOATCHECK("red")
    SQ_FLOATCHECK("green")
    SQ_FLOATCHECK("blue")
#endif

    type = Type::Liquid;

    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);
    gl::EnableVertexAttribArray(0);
    gl::EnableVertexAttribArray(1);

    float points[18];
    float txcrds[12];

    xPT = floatMap["xFlow"] * floatMap["speed"];
    yPT = floatMap["yFlow"] * floatMap["speed"];
    scale = floatMap["scale"];
    wSmooth = floatMap["wSmooth"];
    tinge = {floatMap["red"], floatMap["green"], floatMap["blue"]};
    zPos = floatMap["zPos"];

    float tX = floatMap["xSize"] / scale;
    float tY = floatMap["ySize"] / scale;

    points[5*3+0] = points[1*3+0] = floatMap["xPos"] + floatMap["xSize"];
    txcrds[5*2+0] = txcrds[1*2+0] = tX;
    points[5*3+1] = points[1*3+1] = floatMap["yPos"] + floatMap["ySize"];
    txcrds[5*2+1] = txcrds[1*2+1] = tY;

    points[4*3+0] = points[2*3+0] = floatMap["xPos"];
    txcrds[4*2+0] = txcrds[2*2+0] = 0.f;
    points[4*3+1] = points[2*3+1] = floatMap["yPos"];
    txcrds[4*2+1] = txcrds[2*2+1] = 0.f;

    points[3*3+0] = floatMap["xPos"];
    txcrds[3*2+0] = 0.f;
    points[3*3+1] = floatMap["yPos"] + floatMap["ySize"];
    txcrds[3*2+1] = tY;

    points[0*3+0] = floatMap["xPos"] + floatMap["xSize"];
    txcrds[0*2+0] = tX;
    points[0*3+1] = floatMap["yPos"];
    txcrds[0*2+1] = 0.f;

    points[2] = points[5] = points[8] = points[11] = points[14] = points[17] = zPos;

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
               0, 0, 2.f*zPos, 1};
}

void Liquid::tick() {
    flowOffsetA = flowOffsetB;
    flowOffsetB.x += xPT;
    flowOffsetB.y += yPT;
}
