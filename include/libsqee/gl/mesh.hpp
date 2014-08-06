#pragma once

#include <string>

#include <gl/gl.hpp>

namespace sq {

class Mesh {
public:
    bool load_from_dir(std::string dirPath);

    GLuint vao;
    GLuint texArray;

    GLuint shadVao;
    GLuint shadTex;

    int vCount;
    int texWidth, texHeight;
    float shadN, shadE, shadS, shadW;
private:
    std::vector<glm::vec3> positionsVec;
    std::vector<glm::vec3> normalsVec;
    std::vector<glm::vec2> texcoordsVec;

    sf::Image diffImg;
    sf::Image specImg;
    sf::Image normImg;
    sf::Image shadImg;
};

}
