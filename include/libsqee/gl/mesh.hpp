#pragma once

#include <string>

#include <gl/gl.hpp>

namespace sq {

class Mesh {
public:
    bool load_from_dir(std::string dirPath);

    GLuint vao;
    GLuint texNorm;
    GLuint texDiff;
    GLuint texSpec;

    int vCount;
    bool hasNorm;
    bool hasSpec;
    int texWidth, texHeight;
private:
    std::vector<glm::vec3> pointsVec;
    std::vector<glm::vec3> normalsVec;
    std::vector<glm::vec2> texcoordsVec;

    sf::Image normImg;
    sf::Image diffImg;
    sf::Image specImg;
};

}
