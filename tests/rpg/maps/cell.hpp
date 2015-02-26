#pragma once
#include <sqee/forward.hpp>

#include <map>
#include <memory>
#include <vector>

#include "obj/object.hpp"

namespace sqt {

class HeightLayer {
public:
    HeightLayer(const string& _filePath, glm::ivec2 _minXY, glm::ivec2 _maxXY, float _offs);
    float get_z(uint _x, uint _y) const;
    std::vector<std::vector<float>> floatVV;
};

class Cell {
public:
    Cell(const string& _filePath, const string& _name,
         const std::vector<string>& _loads, glm::ivec2 _posXY, float _posZ);
    const string name;
    const std::vector<string> loads;

    glm::ivec2 minXY; float minZ;
    glm::ivec2 maxXY; float maxZ;
    glm::uvec2 sizeXY; float sizeZ;

    void tick();
    void calc(double _accum);

    glm::vec3 get_min() const;
    glm::vec3 get_max() const;
    glm::vec3 get_size() const;

    std::map<string, HeightLayer> hlMap;
    std::map<string, std::unique_ptr<Object>> objMap;
};

}
