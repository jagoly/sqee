#include "liquid.hpp"

using namespace sqt::obj;

void Liquid::create() {
#ifdef SQEE_DEBUG
    if (!floatMap.count("xPos")) throw;
    if (!floatMap.count("yPos")) throw;
    if (!floatMap.count("xSize")) throw;
    if (!floatMap.count("ySize")) throw;
    if (!floatMap.count("zPos")) throw;
    if (!floatMap.count("xFlow")) throw;
    if (!floatMap.count("yFlow")) throw;
    if (!floatMap.count("speed")) throw;
    if (!floatMap.count("wScale")) throw;
    if (!floatMap.count("wSmooth")) throw;
    if (!floatMap.count("red")) throw;
    if (!floatMap.count("green")) throw;
    if (!floatMap.count("blue")) throw;
#endif

    type = Type::Liquid;

    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);
    gl::EnableVertexAttribArray(0);
    gl::EnableVertexAttribArray(1);

    GLfloat points[12];
    GLfloat txcrds[12];

    zPos = floatMap["zPos"];
    xPT = floatMap["xFlow"] * floatMap["speed"];
    yPT = floatMap["yFlow"] * floatMap["speed"];
    wScale = floatMap["wScale"];
    wSmooth = floatMap["wSmooth"];
    tinge = {floatMap["red"], floatMap["green"], floatMap["blue"]};

    float tX = floatMap["xSize"] / wScale;
    float tY = floatMap["ySize"] / wScale;

    points[0*2+0] = points[4*2+0] = floatMap["xPos"] + floatMap["xSize"];
    txcrds[0*2+0] = txcrds[4*2+0] = tX;
    points[0*2+1] = points[4*2+1] = floatMap["yPos"] + floatMap["ySize"];
    txcrds[0*2+1] = txcrds[4*2+1] = tY;

    points[1*2+0] = points[3*2+0] = floatMap["xPos"];
    txcrds[1*2+0] = txcrds[3*2+0] = 0.f;
    points[1*2+1] = points[3*2+1] = floatMap["yPos"];
    txcrds[1*2+1] = txcrds[3*2+1] = 0.f;

    points[2*2+0] = floatMap["xPos"];
    txcrds[2*2+0] = 0.f;
    points[2*2+1] = floatMap["yPos"] + floatMap["ySize"];
    txcrds[2*2+1] = tY;

    points[5*2+0] = floatMap["xPos"] + floatMap["xSize"];
    txcrds[5*2+0] = tX;
    points[5*2+1] = floatMap["yPos"];
    txcrds[5*2+1] = 0.f;

    GLuint vboP, vboTc;

    gl::GenBuffers(1, &vboP);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboP);
    gl::BufferData(gl::ARRAY_BUFFER, 12 * sizeof(GLfloat), points, gl::STATIC_DRAW);
    gl::VertexAttribPointer(0, 2, gl::FLOAT, false, 0, nullptr);

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
