#pragma once

#include <string>
#include <vector>

namespace sqt {

struct Level {
    Level(std::string filePath);

    unsigned short int width, height;
    unsigned short int pX, pY;

    std::vector<std::vector<unsigned short int>> objects;
    std::vector<std::vector<unsigned short int>> textures;
    std::vector<std::pair<unsigned short int, std::string>> paths;
};

}
