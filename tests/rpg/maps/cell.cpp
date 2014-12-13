#include <fstream>

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
    vector<vector<string>> fileVec(sq::get_words_from_file(filePath));

    vector<ObjectSpec> specVec;

    string section = "";
    for (const vector<string>& line : fileVec) {
        const string& key = line[0];
        if (key[0] == '#') continue;
        if (key == "{") {
            if (!section.empty()) throw;
            section = line[1]; continue;
        }
        if (key == "}") {
            if (section.empty()) throw;
            section.clear(); continue;
        }

        if (section == "header") {
            if (key == "size") {
                ////   CHECK THIS PART ??
                xySize = {stou(line[1]), stou(line[2])};
                size = {stof(line[1]), stof(line[2]), stof(line[3])};
            } else throw; // invalid key
            continue;
        }

        if (section == "heightlayers") {
            glm::ivec2 hlPos(stoi(line[2]) * 4, stoi(line[3]) * 4);
            hlMap.emplace(key, HeightLayer(line[1], hlPos, xySize*4u, stof(line[4])));
            continue;
        }

        if (section == "objects") {
            if (key == "object") {
                if (line[1] == "model")
                    specVec.emplace_back(line[2], ObjType::Model, pos);
                else if (line[1] == "light")
                    specVec.emplace_back(line[2], ObjType::Light, pos);
                else if (line[1] == "liquid")
                    specVec.emplace_back(line[2], ObjType::Liquid, pos);
                else if (line[1] == "data")
                    specVec.emplace_back(line[2], ObjType::Data, pos);
            } else specVec.back().parse_line(line);
            continue;
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
