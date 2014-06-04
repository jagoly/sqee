#pragma once

#include <string>
#include <vector>

namespace sqt {

struct Level {
    Level(std::string);

    unsigned short int width, height;

    std::vector<std::vector<unsigned short int>> objects;
    std::vector<std::vector<unsigned short int>> textures;
    std::vector<std::pair<unsigned short int, std::string>> paths;
};

}
