#pragma once

#include <vector>
#include <string>

#include <libsqee/extra/gl.hpp>

namespace sqt {

class Floor {
public:
    Floor();

    bool load_map(std::string filePath);

    int width, height;

    void get_models(GLuint& vao, int& pCount);

private:
    std::vector<int> tilesModels;
    std::vector<int> tilesCollisions;
    std::vector<int> tilesSwitches;
};

}
