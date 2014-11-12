#include <iostream>
#include <fstream>
#include <algorithm>

#include <sqee/misc/files.hpp>

#include "world.hpp"

#include <ctime>

using namespace sqt::wld;

struct Layout {
    string name;
    string path;
    glm::ivec2 xyPos;
    float zPos;
    vector<string> loads;
};

void World::load_base(const string& _filePath) {
    string filePath = "res/game/worlds/" + _filePath + ".sq_world";

    std::ifstream src;
    src.open(filePath);

#ifdef SQEE_DEBUG
    if (!src.is_open())
        std::cout << "ERROR: Couldn't open file \"" << filePath << "\"" << std::endl;
#endif

    string line;
    string section = "";



    vector<Layout> layoutVec;
    while (std::getline(src, line)) {
        bool blank = true;
        for (const char* c = line.c_str(); *c && blank; c++) {
            blank = (*c == ' ');
        } if (blank) continue;
        char c = line[0];
        if (c == '#') continue;
        if (c == '{') {
            if (!section.empty()) throw; // already in a section
            section = SubStr(line.substr(1)).str; continue;
        }
        if (c == '}') {
            if (section.empty()) throw; // not in a section
            section = ""; continue;
        }

        if (section == "header") {
            //SubStr key(line);
            //if (key.str == "xSize") { size.x = val; continue; }
            //throw; // invalid key
        }

        if (section == "cells") {
            SubStr key(line);
            if (key.str == "cell") {
                layoutVec.emplace_back();
                layoutVec.back().name = SubStr(line, key).str;
            } else
            if (key.str == "path") {
                layoutVec.back().path = SubStr(line, key).str;
            } else
            if (key.str == "xypos") {
                SubStr X(line, key);
                layoutVec.back().xyPos.x = int(X);
                SubStr Y(line, X);
                layoutVec.back().xyPos.y = int(Y);
            } else
            if (key.str == "zpos") {
                layoutVec.back().zPos = float(SubStr(line, key));
            } else
            if (key.str == "loads") {
                SubStr val(line, key);
                while (val.str != "") {
                    layoutVec.back().loads.emplace_back(val.str);
                    val = SubStr(line, val);
                }
            }
        }
    }

    for (Layout& layout : layoutVec) {
        cellMap.emplace(layout.name, CellPair(CData(layout.loads, layout.xyPos, layout.zPos),
                                              Cell(layout.path)));
    }
}

void World::activate_cell(const string& _cell) {
    cellObjects.clear();

    minPos = cellMap.at(_cell).first.pos;
    maxPos = minPos + glm::vec3(cellMap.at(_cell).second.size);
    for (Cell::SOPair& sc : cellMap.at(_cell).second.objectMap)
        cellObjects.emplace_back(cellMap.at(_cell).first.pos, sc.second.get());

    for (const string& s : cellMap.at(_cell).first.loads) {
        minPos = glm::min(minPos, cellMap.at(s).first.pos);
        maxPos = glm::max(maxPos, cellMap.at(s).first.pos + glm::vec3(cellMap.at(s).second.size));
        for (Cell::SOPair& sc : cellMap.at(s).second.objectMap)
            cellObjects.emplace_back(cellMap.at(s).first.pos, sc.second.get());
    }
}

void World::set_player_pos(glm::uvec2 _pos) {
    activate_cell("CellA");
}

void World::tick() {

}

void World::calc(double _accum) {

}
