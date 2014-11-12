#pragma once

#include <unordered_map>

#include "../resbank.hpp"
#include "obj/object.hpp"

namespace sqt {
namespace wld {

class Cell {
public:
    typedef vector<vector<float>> HeightLayer;

    Cell(const string& _filePath);

    void tick();
    void calc(double _accum);

    float get_max16_z(glm::uvec2 _pos, const string& _layer);

    glm::uvec3 size;

    typedef pair<const string, unique_ptr<Object>> SOPair;
    map<const string, unique_ptr<Object>> objectMap;

private:
    map<string, HeightLayer> hlMap;
    map<pair<string, pair<int, int>>, pair<string, pair<int, int>>> joinMap;
};

}
}
