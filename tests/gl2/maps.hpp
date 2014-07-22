#pragma once

#include <vector>
#include <string>

namespace sqt {

class Floor {
public:
    Floor();

    bool load_map(std::string filePath);

    short int* get_tilesModels;
    short int* get_tilesCollisions;
    short int* get_tilesSwitches;
private:
    std::vector<short int> tilesModels;
    std::vector<short int> tilesCollisions;
    std::vector<short int> tilesSwitches;
};

}
