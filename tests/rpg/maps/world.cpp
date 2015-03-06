#include <array>

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <sqee/gl/gl_ext_3_3.hpp>
#include <sqee/gl/maths.hpp>
#include <sqee/app/application.hpp>
#include <sqee/app/logging.hpp>
#include <sqee/misc/files.hpp>
#include <sqee/misc/strtonum.hpp>

#include "../scenegame.hpp"
#include "world.hpp"

using namespace sqt;

World::World(sq::Application& _app) : app(_app) {
    ubo.reserve("ambiColour", 3);
    ubo.reserve("skylEnable", 1);
    ubo.reserve("skylDir", 4);
    ubo.reserve("skylColour", 4);
    ubo.reserve("skylMatArr", 16*6);
    ubo.reserve("spotCount", 4);
    ubo.reserve("spotArr", 76*8);
    ubo.create();
}

void World::load_base(const string& _filePath) {
    string filePath = "res/game/worlds/" + _filePath + ".sq_world";
    std::vector<std::vector<string>> fileVec(sq::get_words_from_file(filePath));

    struct Spec {
        Spec(const string& _name) : name(_name) {}
        string name, path;
        glm::vec3 pos;
        std::vector<string> loads;
    };
    std::vector<Spec> specVec;

    std::vector<std::pair<string, std::vector<std::pair<string, string>>>> hlVec;

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
            // empty
            throw;
        }

        if (section == "defaults") {
            if (key == "ambiColour")
                ambiColour = {stof(line[1]), stof(line[2]), stof(line[3])};
            else if (key == "skylEnable")
                skylEnable = line[1] == "true";
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
            else if (key == "pos")
                specVec.back().pos = {stof(line[1]), stof(line[2]), stof(line[3])};
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

    for (const auto& spec : specVec)
        cellMap.emplace(spec.name, Cell(spec.path, spec.name, spec.loads, spec.pos));

    for (const auto& val : hlVec)
        hlMap.insert(val);

    set_active_cell("CellA");
}

float World::get_maxZ4(const string& _layer, int _x, int _y) {
//    float val = -INFINITY;
//    bool a=false, b=false, c=false, d=false;
//    for (auto& ssPair : hlMap.at(_layer)) {
//        if (a && b && c && d) break;

//        const Cell& cell = cellMap.at(ssPair.first);
//        const HeightLayer& hl = cell.hlMap.at(ssPair.second);
//        glm::ivec2 xy1 = cell.minXY * 4;
//        glm::ivec2 xy2 = xy1 + glm::ivec2(cell.sizeXY) * 4;

//        if (!a && _x >= xy1.x && _y >= xy1.y && _x < xy2.x && _y < xy2.y) {
//            val = std::max(val, hl.get_z(_x-xy1.x, _y-xy1.y)); a = true;
//        }
//        if (!b && _x+1 >= xy1.x && _y >= xy1.y && _x+1 < xy2.x && _y < xy2.y) {
//            val = std::max(val, hl.get_z(_x+1-xy1.x, _y-xy1.y)); b = true;
//        }
//        if (!c && _x >= xy1.x && _y+1 >= xy1.y && _x < xy2.x && _y+1 < xy2.y) {
//            val = std::max(val, hl.get_z(_x+1-xy1.x, _y+1-xy1.y)); c = true;
//        }
//        if (!d && _x+1 >= xy1.x && _y+1 >= xy1.y && _x+1 < xy2.x && _y+1 < xy2.y) {
//            val = std::max(val, hl.get_z(_x+1-xy1.x, _y+1-xy1.y)); d = true;
//        }
//    }
//    return val;

    return 0.f;
}

void World::set_active_cell(const string& _cell) {
    objectList.clear();

    const Cell& activeCell = cellMap.at(_cell);
    for (const auto& so : activeCell.objMap) {
        Object* ptr = so.second.get();
        objectList.push_front(ptr);
//        if (ptr->type == ObjType::Model)
//            modelList.push_front(static_cast<Model*>(ptr));
//        else if (ptr->type == ObjType::Liquid)
//            liquidList.push_front(static_cast<Liquid*>(ptr));
//        else if (ptr->type == ObjType::Reflector)
//            reflectorList.push_front(static_cast<Reflector*>(ptr));
//        else if (ptr->type == ObjType::Light && lightVec.size() < 8)
//            lightVec.push_back(static_cast<Light*>(ptr));
    }

    for (const auto& cellName : activeCell.loads) {
        const Cell& iterCell = cellMap.at(cellName);
        for (const auto& so : iterCell.objMap) {
            Object* ptr = so.second.get();
            objectList.push_front(ptr);
//            if (ptr->type == ObjType::Model)
//                modelList.push_front(static_cast<Model*>(ptr));
//            else if (ptr->type == ObjType::Liquid)
//                liquidList.push_front(static_cast<Liquid*>(ptr));
//            else if (ptr->type == ObjType::Reflector)
//                reflectorList.push_front(static_cast<Reflector*>(ptr));
//            else if (ptr->type == ObjType::Light && lightVec.size() < 8)
//                lightVec.push_back(static_cast<Light*>(ptr));
        }
    }



    //spotCount = lightVec.size();

    ubo.bind(1);
    ubo.update("ambiColour", &ambiColour);
    ubo.update("skylEnable", &skylEnable);
    ubo.update("skylDir", &skylDir);
    ubo.update("skylColour", &skylColour);

    ubo.update("spotCount", &spotCount);
//    for (uint i = 0; i < lightVec.size(); i++) {
//        const Light* light = lightVec[i];
//        ubo.update("spots", &light->pos,       i*12+0, 3);
//        ubo.update("spots", &light->angle,     i*12+3, 1);
//        ubo.update("spots", &light->dir,       i*12+4, 3);
//        ubo.update("spots", &light->intensity, i*12+7, 1);
//        ubo.update("spots", &light->colour,    i*12+8, 3);
//        ubo.update("spots", &light->softness,  i*12+11, 1);
//        ubo.update("spotMats", &light->shadMat, i*16, 16);
//    }

    updateScene = true;
}

void World::set_active_tile(glm::ivec2 _tile) {
//    const Cell* cellPtr;
//    for (auto& scPair : cellMap) {
//        cellPtr = &scPair.second;
//        if (sq::within_box(_tile, cellPtr->minXY, cellPtr->maxXY))
//            break;
//    }
//    set_active_cell(cellPtr->name);
}

void World::tick() {
    static sq::Camera& cam = static_cast<SceneGame*>(app.sceneIM.get("game").get())->camera;

    skylMatVec.clear();

    glm::vec3 t1(glm::cross(skylDir, {0, 1, 0})), t2(glm::cross(skylDir, {0, 1, 0}));
    glm::vec3 tangent = glm::normalize(glm::length(t1) > glm::length(t2) ? t1 : t2);
    for (auto& csm : cam.csmVecA) {\
        glm::vec3 frusCentre = sq::calc_frusCentre(csm.second);
        glm::mat4 viewMat = glm::lookAt(frusCentre-skylDir, frusCentre, tangent);
        glm::vec3 arr[8] {
            glm::vec3(viewMat * glm::vec4(csm.second.xyz, 1)),
            glm::vec3(viewMat * glm::vec4(csm.second.xyZ, 1)),
            glm::vec3(viewMat * glm::vec4(csm.second.xYz, 1)),
            glm::vec3(viewMat * glm::vec4(csm.second.xYZ, 1)),
            glm::vec3(viewMat * glm::vec4(csm.second.Xyz, 1)),
            glm::vec3(viewMat * glm::vec4(csm.second.XyZ, 1)),
            glm::vec3(viewMat * glm::vec4(csm.second.XYz, 1)),
            glm::vec3(viewMat * glm::vec4(csm.second.XYZ, 1))
        };

        glm::vec3 minO = {INFINITY, INFINITY, INFINITY};
        glm::vec3 maxO = {-INFINITY, -INFINITY, -INFINITY};
        for (auto& vec : arr) {
            minO = glm::min(minO, vec);
            maxO = glm::max(maxO, vec);
        }

        glm::mat4 projMat = sq::make_orthMat(minO, maxO);
        skylMatVec.push_back(projMat * viewMat);
    }

    ubo.bind(1);
    ubo.update("skylMatArr", skylMatVec.data(), 0, skylMatVec.size()*16);

    for (auto& scPair : cellMap)
        scPair.second.tick();
}

void World::calc(double _accum) {
    for (auto& scPair : cellMap)
        scPair.second.calc(_accum);

    if (app.settings.check_update("World")) updateScene = true;
}
