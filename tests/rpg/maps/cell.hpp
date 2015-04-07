#pragma once
#include <sqee/forward.hpp>

#include <map>
#include <memory>
#include <vector>

#include "obj/object.hpp"

namespace sqt {

class Cell {
public:
    Cell(const string& _filePath, const string& _name, const std::vector<string>& _loads, vec3 _pos);
    const string name;
    const std::vector<string> loads;

    glm::vec3 pos, size;

    void tick();
    void calc(double _accum);

    std::map<string, unique_ptr<Object>> objMap;
};

}
