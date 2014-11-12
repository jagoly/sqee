#include <iostream>
#include <fstream>

#include <sqee/misc/files.hpp>

#include "cell.hpp"

using namespace sqt::wld;

Cell::Cell(const string& _filePath) {
    string filePath = "res/game/cells/" + _filePath + ".sq_cell";

    std::ifstream src;
    src.open(filePath);

    #ifdef SQEE_DEBUG
    if (!src.is_open())
        std::cout << "ERROR: Couldn't open file \"" << filePath << "\"" << std::endl;
    #endif

    string line;
    string section = "";

    vector<ObjectSpec> specVec;
    while (std::getline(src, line)) {
        bool blank = true;
        for (const char* c = line.c_str(); *c && blank; c++) {
            blank = (*c == ' ');
        } if (blank) continue;
        else {
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
        }

        if (section == "header") {
            SubStr key(line);
            if (key.str == "size") {
                SubStr X(line, key); SubStr Y(line, X); SubStr Z(line, Y);
                size = {uint(X), uint(Y), uint(Z)};
                continue;
            }
            throw; // invalid key
        }

        if (section == "layers") {
            SubStr uid(line);
            SubStr path(line, uid);
            SubStr X(line, path);
            SubStr Y(line, X);
            SubStr Z(line, Y);

            HeightLayer& hl = hlMap[uid.str];

            std::ifstream src2;
            string hlPath = "res/maps/heightlayers/" + path.str + ".sq_hl";
            src2.open(hlPath, std::ios::binary);

            #ifdef SQEE_DEBUG
            if (!src2.is_open())
                std::cout << "ERROR: Couldn't open file \"" << hlPath << "\"" << std::endl;
            #endif

            int xSize;
            int ySize;
            src2.read((char*)&xSize, 4);
            src2.read((char*)&ySize, 4);

            float fBuf;
            for (uint y = 0; y < size.y*4; y++) {
                hl.emplace_back();
                for (uint x = 0; x < size.x*4; x++) {
                    if (y < uint(Y)*4 || y >= (uint(Y)*4 + ySize) ||
                        x < uint(X)*4 || x >= (uint(X)*4 + xSize)) {
                        hl.back().emplace_back(-INFINITY);
                    } else {
                        src2.read((char*)&fBuf, 4);
                        hl.back().emplace_back(float(Z) + fBuf);
                    }
                }
            }
            continue;
        }

        if (section == "objects") {
            SubStr key(line);
            if (key.str == "object")
                specVec.emplace_back();
            specVec.back().parse_line(line);
        }
    }


    for (ObjectSpec& spec : specVec) {
        Object* ptr;
        if (spec.objType == ObjType::Model) {
            ptr = new Model(spec.objType, spec.uid);
            ptr->create(spec);
        }
        else
        if (spec.objType == ObjType::Liquid) {
            ptr = new Liquid(spec.objType, spec.uid);
            ptr->create(spec);
        }
        objectMap.emplace(spec.uid, std::unique_ptr<Object>(ptr));
    }
}

void Cell::tick() {
    for (SOPair& o : objectMap) {
        o.second->tick();
    }
}

void Cell::calc(double _accum) {
    for (SOPair& o : objectMap) {
        o.second->calc(_accum);
    }
}

float Cell::get_max16_z(glm::uvec2 _pos, const string& _layer) {
    HeightLayer& hl = hlMap[_layer];

    float val = -INFINITY;
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            val = std::max(val, hl[_pos.y+y][_pos.x+x]);

    return val;
}

