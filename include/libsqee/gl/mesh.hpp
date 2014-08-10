#pragma once

#include <string>

#include <gl/gl.hpp>

namespace sq {

const GLfloat shadowTexcoords[12] = {
    0.f, 0.f,  1.f, 1.f,  0.f, 1.f,
    0.f, 0.f,  1.f, 1.f,  1.f, 0.f
};

class Mesh {
public:
    bool load_from_dir(std::string dirPath);

    GLuint vao;
    GLuint texNorm;
    GLuint texDiff;
    GLuint texAmbi;
    GLuint texSpec;

    GLuint vaoShadow;
    GLuint texShadow;

    int vCount;
    bool hasNorm;
    bool hasAmbi;
    bool hasSpec;
    bool hasShadow;
    int texWidth, texHeight;
private:
    std::vector<glm::vec3> pointsVec;
    std::vector<glm::vec3> normalsVec;
    std::vector<glm::vec2> texcoordsVec;
    std::vector<glm::vec3> shadowVec;

    sf::Image normImg;
    sf::Image diffImg;
    sf::Image ambiImg;
    sf::Image specImg;
    sf::Image shadowImg;
};

}
