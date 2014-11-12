#pragma once

#include "../resbank.hpp"
#include "cell.hpp"
#include "obj/object.hpp"

namespace sqt {
namespace wld {

class World {
public:
    struct CData {
        CData(const vector<string>& _loads, glm::ivec2 _xyPos, float _zPos)
            : loads(_loads), xyPos(_xyPos), zPos(_zPos), pos(xyPos.x, xyPos.y, zPos) {}
        vector<string> loads;
        glm::ivec2 xyPos; float zPos;
        glm::vec3 pos;
    };

    void load_base(const string& _filePath);

    void tick();
    void calc(double _accum);

    void activate_cell(const string& _cell);
    string get_move(glm::uvec2 _prev, glm::uvec2 _next);
    void set_player_pos(glm::uvec2 _pos);

    glm::vec3 minPos, maxPos;
    glm::uvec2 playerPos = {0, 0};

    typedef pair<glm::vec3&, Object*> OPair;
    list<OPair> cellObjects;

private:
    typedef pair<CData, Cell> CellPair;
    map<const string, CellPair> cellMap;
   // map<const string, vector<Cell::HeightLayer&>> hlMap;
};

}
}
