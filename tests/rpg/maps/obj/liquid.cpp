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

    GLfloat points[18];
    GLfloat txcrds[12];

    xPT = floatMap["xFlow"] * floatMap["speed"];
    yPT = floatMap["yFlow"] * floatMap["speed"];
    scale = floatMap["scale"];
    wSmooth = floatMap["wSmooth"];
    tinge = {floatMap["red"], floatMap["green"], floatMap["blue"]};

    float tX = floatMap["xSize"] / scale;
    float tY = floatMap["ySize"] / scale;

    points[0*3+0] = points[4*3+0] = floatMap["xPos"] + floatMap["xSize"];
    txcrds[0*2+0] = txcrds[4*2+0] = tX;
    points[0*3+1] = points[4*3+1] = floatMap["yPos"] + floatMap["ySize"];
    txcrds[0*2+1] = txcrds[4*2+1] = tY;

    points[1*3+0] = points[3*3+0] = floatMap["xPos"];
    txcrds[1*2+0] = txcrds[3*2+0] = 0.f;
    points[1*3+1] = points[3*3+1] = floatMap["yPos"];
    txcrds[1*2+1] = txcrds[3*2+1] = 0.f;

    points[2*3+0] = floatMap["xPos"];
    txcrds[2*2+0] = 0.f;
    points[2*3+1] = floatMap["yPos"] + floatMap["ySize"];
    txcrds[2*2+1] = tY;

    points[5*3+0] = floatMap["xPos"] + floatMap["xSize"];
    txcrds[5*2+0] = tX;
    points[5*3+1] = floatMap["yPos"];
    txcrds[5*2+1] = 0.f;

    points[0+2] = points[3+2] = points[6+2] = points[9+2] = points[12+2] = points[15+2] = floatMap["zPos"];

    GLuint vboP, vboTc;

    gl::GenBuffers(1, &vboP);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboP);
    gl::BufferData(gl::ARRAY_BUFFER, 18 * sizeof(GLfloat), points, gl::STATIC_DRAW);
    gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 0, nullptr);

    gl::GenBuffers(1, &vboTc);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboTc);
    gl::BufferData(gl::ARRAY_BUFFER, 12 * sizeof(GLfloat), txcrds, gl::STATIC_DRAW);
    gl::VertexAttribPointer(1, 2, gl::FLOAT, false, 0, nullptr);
}

void Liquid::tick(int) {
    flowOffsetA = flowOffsetB;
    flowOffsetB.x += xPT;
    flowOffsetB.y += yPT;
}
