#include <sqee/misc/files.hpp>

#include "../resbank.hpp"
#include "../settings.hpp"
#include "world.hpp"

using namespace sqt;

World::World() {
    ubo.reserve("ambiColour", 3);
    ubo.reserve("skylEnable", 1);
    ubo.reserve("skylDir", 4);
    ubo.reserve("skylColour", 4);
    ubo.reserve("skylMat", 16);
    ubo.reserve("spotCount", 4);
    ubo.reserve("spots", 12*8);
    ubo.reserve("spotMats", 16*8);
    ubo.create();
}

void World::load_base(const string& _filePath) {
    string filePath = "res/game/worlds/" + _filePath + ".sq_world";
    vector<vector<string>> fileVec(sq::get_words_from_file(filePath));

    struct Spec {
        Spec(const string& _name) : name(_name) {}
        string name, path;
        glm::ivec2 xyPos;
        float zPos;
        vector<string> loads;
    };
    vector<Spec> specVec;

    vector<pair<string, vector<pair<string, string>>>> hlVec;

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
            // empty
            throw;
        }

        if (section == "defaults") {
            if (key == "ambiColour")
                ambiColour = {stof(line[1]), stof(line[2]), stof(line[3])};
            else if (key == "skylEnable")
                skylEnable = line[1] == "true";
            else if (key == "skylTexSize")
                skylTexSize = stoi(line[1]);
            else if (key == "skylDir")
                skylDir = {stof(line[1]), stof(line[2]), stof(line[3])};
            else if (key == "skylColour")
                skylColour = {stof(line[1]), stof(line[2]), stof(line[3])};
            else throw;
            continue;
        }

        if (section == "cells") {
            if (key == "cell")
                specVec.emplace_back(line[1]);
            else if (key == "path")
                specVec.back().path = line[1];
            else if (key == "xyPos")
                specVec.back().xyPos = {stou(line[1]), stou(line[2])};
            else if (key == "zPos")
                specVec.back().zPos = std::stof(line[1]);
            else if (key == "loads")
                for (uint i = 1; i < line.size(); i++)
                    specVec.back().loads.emplace_back(line[i]);
            else throw;
            continue;
        }

        if (section == "heightlayers") {
            if (key == "layer") hlVec.push_back({line[1], {}});
            else hlVec.back().second.emplace_back(line[0], line[1]);
            continue;
        }
    }

    for (const Spec& spec : specVec)
        cellMap.emplace(spec.name, Cell(spec.path, spec.name, spec.loads, spec.xyPos, spec.zPos));

    for (const pair<string, vector<pair<string, string>>>& val : hlVec)
        hlMap.insert(val);

    set_active_cell("CellA");
}

float World::get_maxZ4(const string& _layer, int _x, int _y) {
    return 0;
    float val = -INFINITY;
    bool a=false, b=false, c=false, d=false;
    for (pair<string, string>& ssPair : hlMap.at(_layer)) {
        if (a && b && c && d) break;

        const Cell& cell = cellMap.at(ssPair.first);
        const HeightLayer& hl = cell.hlMap.at(ssPair.second);
        glm::ivec2 xy1 = cell.minXY * 4;
        glm::ivec2 xy2 = xy1 + glm::ivec2(cell.sizeXY) * 4;

        if (!a && _x >= xy1.x && _y >= xy1.y && _x < xy2.x && _y < xy2.y) {
            val = std::max(val, hl.get_z(_x-xy1.x, _y-xy1.y)); a = true;
        }
        if (!b && _x+1 >= xy1.x && _y >= xy1.y && _x+1 < xy2.x && _y < xy2.y) {
            val = std::max(val, hl.get_z(_x+1-xy1.x, _y-xy1.y)); b = true;
        }
        if (!c && _x >= xy1.x && _y+1 >= xy1.y && _x < xy2.x && _y+1 < xy2.y) {
            val = std::max(val, hl.get_z(_x+1-xy1.x, _y+1-xy1.y)); c = true;
        }
        if (!d && _x+1 >= xy1.x && _y+1 >= xy1.y && _x+1 < xy2.x && _y+1 < xy2.y) {
            val = std::max(val, hl.get_z(_x+1-xy1.x, _y+1-xy1.y)); d = true;
        }
    }
    return val;
}

void World::set_active_cell(const string& _cell) {
    objectList.clear();
    modelList.clear();
    liquidList.clear();
    reflectorList.clear();
    lightVec.clear();

    const Cell& activeCell = cellMap.at(_cell);
    minPos = activeCell.get_min();
    maxPos = activeCell.get_max();
    for (const Cell::SOPair& so : activeCell.objectMap) {
        Object* ptr = so.second.get();
        objectList.push_front(ptr);
        if (ptr->type == ObjType::Model)
            modelList.push_front(static_cast<Model*>(ptr));
        else if (ptr->type == ObjType::Liquid)
            liquidList.push_front(static_cast<Liquid*>(ptr));
        else if (ptr->type == ObjType::Reflector)
            reflectorList.push_front(static_cast<Reflector*>(ptr));
        else if (ptr->type == ObjType::Light && lightVec.size() < 8)
            lightVec.push_back(static_cast<Light*>(ptr));
    }

    for (const string& cellName : activeCell.loads) {
        const Cell& iterCell = cellMap.at(cellName);
        minPos = glm::min(minPos, iterCell.get_min());
        maxPos = glm::max(maxPos, iterCell.get_max());
        for (const Cell::SOPair& so : iterCell.objectMap) {
            Object* ptr = so.second.get();
            objectList.push_front(ptr);
            if (ptr->type == ObjType::Model)
                modelList.push_front(static_cast<Model*>(ptr));
            else if (ptr->type == ObjType::Liquid)
                liquidList.push_front(static_cast<Liquid*>(ptr));
            else if (ptr->type == ObjType::Reflector)
                reflectorList.push_front(static_cast<Reflector*>(ptr));
            else if (ptr->type == ObjType::Light && lightVec.size() < 8)
                lightVec.push_back(static_cast<Light*>(ptr));
        }
    }


    /// Lighting
    glm::vec3 centre = (minPos + maxPos) / 2.f;
    glm::mat4 viewMat = glm::lookAt(centre, centre + skylDir, {0, 0, 1});

    array<glm::vec3, 8> arr = {
        glm::vec3(viewMat * glm::vec4(minPos.x, minPos.y, maxPos.z, 1)),
        glm::vec3(viewMat * glm::vec4(minPos.x, minPos.y, minPos.z, 1)),
        glm::vec3(viewMat * glm::vec4(minPos.x, maxPos.y, minPos.z, 1)),
        glm::vec3(viewMat * glm::vec4(minPos.x, maxPos.y, maxPos.z, 1)),
        glm::vec3(viewMat * glm::vec4(maxPos.x, minPos.y, maxPos.z, 1)),
        glm::vec3(viewMat * glm::vec4(maxPos.x, minPos.y, minPos.z, 1)),
        glm::vec3(viewMat * glm::vec4(maxPos.x, maxPos.y, minPos.z, 1)),
        glm::vec3(viewMat * glm::vec4(maxPos.x, maxPos.y, maxPos.z, 1))
    };

    glm::vec3 minO = centre, maxO = centre;
    for (glm::vec3& vec : arr) {
        minO = glm::min(minO, vec);
        maxO = glm::max(maxO, vec);
    }

    glm::mat4 projMat = glm::ortho(minO.x, maxO.x, minO.y, maxO.y, minO.z, maxO.z);
    skylMat = projMat * viewMat;

    spotCount = lightVec.size();

    ubo.bind(1);
    ubo.update("ambiColour", &ambiColour);
    ubo.update("skylEnable", &skylEnable);
    ubo.update("skylDir", &skylDir);
    ubo.update("skylColour", &skylColour);
    ubo.update("skylMat", &skylMat);

    ubo.update("spotCount", &spotCount);
    for (uint i = 0; i < lightVec.size(); i++) {
        const Light* light = lightVec[i];
        ubo.update("spots", &light->pos,       i*12+0, 3);
        ubo.update("spots", &light->angle,     i*12+3, 1);
        ubo.update("spots", &light->dir,       i*12+4, 3);
        ubo.update("spots", &light->intensity, i*12+7, 1);
        ubo.update("spots", &light->colour,    i*12+8, 3);
        ubo.update("spots", &light->softness,  i*12+11, 1);
        ubo.update("spotMats", &light->shadMat, i*16, 16);
    }

    updateScene = true;
}

void World::set_active_tile(glm::ivec2 _tile) {
    const Cell* cellPtr;
    for (pair<const string, Cell>& scPair : cellMap) {
        cellPtr = &scPair.second;
        if (sq::within_box(_tile, cellPtr->minXY, cellPtr->maxXY))
            break;
    }
    set_active_cell(cellPtr->name);
}

void World::tick() {
    for (pair<const string, Cell>& scPair : cellMap)
        scPair.second.tick();
}

void World::calc(double _accum) {
    for (pair<const string, Cell>& scPair : cellMap)
        scPair.second.calc(_accum);

    if (vidSet().check_update("World")) updateScene = true;
}
