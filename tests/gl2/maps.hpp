#pragma once

#include <vector>
#include <string>

#include <libsqee/extra/gl.hpp>
#include <libsqee/textureholder.hpp>

namespace sqt {

class Ground {
public:
    Ground();

    bool load_map(std::string filePath);

    int width, height;
    int texCount;

    void get_models(GLuint& vao, GLuint& texArray, int& pCount);

private:
    std::vector<int> tilesModels;
    std::vector<int> tilesCollisions;
    std::vector<int> tilesSwitches;
    std::vector<int> tilesTextures;
    std::vector<std::string> texPaths;
};

}
