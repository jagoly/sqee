#include <fstream>

#include <sqee/gl/gl_ext_3_3.hpp>
#include <sqee/app/logging.hpp>
#include <sqee/misc/files.hpp>

#include "obj/model.hpp"
#include "obj/liquid.hpp"
#include "obj/light.hpp"
#include "obj/reflector.hpp"

#include "cell.hpp"

using namespace sqt;

HeightLayer::HeightLayer(const string& _filePath, glm::ivec2 _min,
                         glm::ivec2 _max, float _offs)
{
    const string filePath = "res/game/heightlayers/" + _filePath + ".sq_hl";
    std::ifstream src(filePath, std::ios::binary);

    #ifdef SQEE_DEBUG
    if (!src.is_open())
        sq::log_error("Couldn't open file $0", filePath);
    #endif

    int xSize, ySize;
    src.read((char*)&xSize, 4);
    src.read((char*)&ySize, 4);

    float fBuf;
    for (int y = 0; y < _max.y; y++) {
        floatVV.emplace_back();
        for (int x = 0; x < _max.x; x++) {
            if (x < _min.x || x >= _min.x + xSize || y < _min.y || y >= _min.y + ySize)
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
           const std::vector<string>& _loads, glm::vec3 _pos)
    : name(_name), loads(_loads), pos(_pos)
{
    string filePath = "res/game/cells/" + _filePath + ".sq_cell";
    std::vector<std::vector<string>> fileVec(sq::get_words_from_file(filePath));

    std::vector<ObjSpec> specVec;

    string section = "";
    for (const std::vector<string>& line : fileVec) {
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
            if (key == "size")
                size = {stoi(line[1]), stoi(line[2]), stoi(line[3])};
            else throw; // invalid key
            continue;
        }

        if (section == "heightlayers") {
            //glm::ivec2 hlPos(stoi(line[2]) * 4, stoi(line[3]) * 4);
            //hlMap.emplace(key, HeightLayer(line[1], hlPos, sizeXY*4u, stof(line[4])));
            continue;
        }

        if (section == "objects") {
            if (key == "object") {
                if (line[1] == "model")
                    specVec.emplace_back(line[2], ObjType::Model, *this);
                else if (line[1] == "liquid")
                    specVec.emplace_back(line[2], ObjType::Liquid, *this);
                else if (line[1] == "reflector")
                    specVec.emplace_back(line[2], ObjType::Reflector, *this);
                else if (line[1] == "light")
                    specVec.emplace_back(line[2], ObjType::Light, *this);
            } else specVec.back().parse_line(line);
            continue;
        }
    }

    for (ObjSpec& spec : specVec) {
        Object* ptr = nullptr;
        if (spec.type == ObjType::Model)
            ptr = new Model(spec);
        else if (spec.type == ObjType::Light)
            ptr = new Light(spec);
        else if (spec.type == ObjType::Liquid)
            ptr = new Liquid(spec);
        else if (spec.type == ObjType::Reflector)
            ptr = new Reflector(spec);
        objMap.emplace(spec.name, std::unique_ptr<Object>(ptr));
    }
}

void Cell::tick() {
    for (auto& strObj : objMap) {
        strObj.second->tick();
    }
}

void Cell::calc(double _accum) {
    for (auto& strObj : objMap) {
        strObj.second->calc(_accum);
    }
}
