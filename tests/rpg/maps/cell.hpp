#pragma once

#include <unordered_map>

#include "obj/object.hpp"

namespace sqt {
namespace wld {

class HeightLayer {
public:
    HeightLayer(const string& _filePath, glm::ivec2 _pos, glm::uvec2 _size, float _offs);
    float get_z(uint _x, uint _y) const;
    vector<vector<float>> floatVV;
};

class Cell {
public:
    Cell(const string& _filePath, const string& _name,
         const vector<string>& _loads, glm::ivec2 _xyPos, float _zPos);
    const string name;
    const vector<string> loads;
    glm::ivec2 xyPos;
    glm::uvec2 xySize;

    void tick();
    void calc(double _accum);

    glm::vec3 pos, size;

    map<string, HeightLayer> hlMap;
    map<string, unique_ptr<Object>> objectMap;
    typedef pair<const string, unique_ptr<Object>> SOPair;

private:
};

}
}
