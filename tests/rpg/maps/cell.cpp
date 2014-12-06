#include <iostream>
#include <fstream>
#include <sstream>

#include <sqee/misc/files.hpp>

#include "../resbank.hpp"
#include "cell.hpp"

using namespace sqt::wld;

HeightLayer::HeightLayer(const string& _filePath, glm::ivec2 _pos, glm::uvec2 _size, float _offs) {
    const string filePath = "res/game/heightlayers/" + _filePath + ".sq_hl";
    std::ifstream src(filePath, std::ios::binary);

    #ifdef SQEE_DEBUG
    if (!src.is_open())
        cout << "ERROR: Couldn't open file \"" << filePath << "\"" << endl;
    #endif

    int xSize, ySize;
    src.read((char*)&xSize, 4);
    src.read((char*)&ySize, 4);

    float fBuf;
    for (int y = 0; y < int(_size.y); y++) {
        floatVV.emplace_back();
        for (int x = 0; x < int(_size.x); x++) {
            if (x < _pos.x || x >= _pos.x + xSize || y < _pos.y || y >= _pos.y + ySize)
                floatVV.back().emplace_back(5.f);
            else {
                src.read((char*)&fBuf, 4);
                floatVV.back().emplace_back(_offs + fBuf);
            }
        }
    }
}

float HeightLayer::get_z(uint _x, uint _y) const {
    return floatVV[_y][_x];
}

Cell::Cell(const string& _filePath, const string& _name,
           const vector<string>& _loads, glm::ivec2 _xyPos, float _zPos)
    : name(_name), loads(_loads), xyPos(_xyPos), pos(xyPos.x, xyPos.y, _zPos) {
    string filePath = "res/game/cells/" + _filePath + ".sq_cell";
    std::ifstream src(filePath);

    #ifdef SQEE_DEBUG
    if (!src.is_open())
        cout << "ERROR: Couldn't open file \"" << filePath << "\"" << endl;
    #endif

    vector<ObjectSpec> specVec;

    string line;
    string section = "";
    while (std::getline(src, line)) {
        vector<string> vec;
        {   std::stringstream stream(line); string val;
            while (stream >> val) vec.emplace_back(val);
        }

        if (vec.empty() || vec[0][0] == '#') continue;

        if (vec[0] == "{") {
            if (!section.empty()) throw; // already in a section
            section = vec[1]; continue;
        }
        if (vec[0] == "}") {
            if (section.empty()) throw; // not in a section
            section = ""; continue;
        }

        if (section == "header") {
            string& key = vec[0];;
            if (key == "size") {
                xySize = {std::stoi(vec[1]), std::stoi(vec[2])};
                size = {std::stof(vec[1]), std::stof(vec[2]), std::stoi(vec[3])};
                continue;
            }
            throw; // invalid key
        }

        if (section == "heightlayers") {
            glm::ivec2 hlpos(std::stoi(vec[2]) * 4, std::stoi(vec[3]) * 4);
            glm::uvec2 hlsize(size.x * 4, size.y * 4);
            float hloffs(std::stof(vec[4]));
            hlMap.emplace(vec[0], HeightLayer(vec[1], hlpos, hlsize, hloffs));
            continue;
        }

        if (section == "objects") {
            if (vec[0] == "object") {
                if (vec[1] == "model")
                    specVec.emplace_back(vec[2], ObjType::Model, pos);
                else if (vec[1] == "light")
                    specVec.emplace_back(vec[2], ObjType::Light, pos);
                else if (vec[1] == "liquid")
                    specVec.emplace_back(vec[2], ObjType::Liquid, pos);
                else if (vec[1] == "data")
                    specVec.emplace_back(vec[2], ObjType::Data, pos);
            } else specVec.back().parse_line(vec);
        }
    }


    for (ObjectSpec& spec : specVec) {
        Object* ptr;
        if (spec.type == ObjType::Model)
            ptr = new Model(spec);
        else if (spec.type == ObjType::Light)
            ptr = new Light(spec);
        else if (spec.type == ObjType::Liquid)
            ptr = new Liquid(spec);
        else if (spec.type == ObjType::Data)
            ptr = new Data(spec);
        objectMap.emplace(spec.name, std::unique_ptr<Object>(ptr));
    }
}

void Cell::tick() {
    for (SOPair& so : objectMap) {
        so.second->tick();
    }
}

void Cell::calc(double _accum) {
    for (SOPair& so : objectMap) {
        so.second->calc(_accum);
    }
}
