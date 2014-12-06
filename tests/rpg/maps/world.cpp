#include <fstream>
#include <sstream>

#include <sqee/misc/files.hpp>

#include "../resbank.hpp"
#include "../settings.hpp"
#include "world.hpp"

using namespace sqt::wld;

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

    slFb.create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32);
    for (sq::Framebuffer& fb : spFbArr)
        fb.create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT32);
}

void World::load_base(const string& _filePath) {
    string filePath = "res/game/worlds/" + _filePath + ".sq_world";
    std::ifstream src(filePath);

    #ifdef SQEE_DEBUG
    if (!src.is_open())
        cout << "ERROR: Couldn't open file \"" << filePath << "\"" << endl;
    #endif

    struct Spec {
        string name, path;
        glm::ivec2 xyPos;
        float zPos;
        vector<string> loads;
    };
    vector<Spec> specVec;

    vector<pair<string, vector<pair<string, string>>>> hlVec;

    string line;
    string section = "";
    while (std::getline(src, line)) {
        vector<string> vec;
        {   std::stringstream stream(line); string val;
            while (stream >> val) vec.emplace_back(val);
        }

        if (vec.empty() || vec[0] == "#") continue;

        if (vec[0] == "{") {
            if (!section.empty()) throw; // already in a section
            section = vec[1]; continue;
        }
        if (vec[0] == "}") {
            if (section.empty()) throw; // not in a section
            section = ""; continue;
        }

        if (section == "header") {
            // empty
            throw;
        }

        if (section == "defaults") {
            const string& key = vec[0];
            if (key == "ambiColour") {
                ambiColour.r = std::stoi(vec[1]);
                ambiColour.g = std::stof(vec[2]);
                ambiColour.b = std::stof(vec[3]);
            } else if (key == "skylEnable") {
                skylEnable = vec[1] == "true";
            } else if (key == "skylTexSize") {
                skylTexSize = std::stoi(vec[1]);
            } else if (key == "skylDir") {
                skylDir.x = std::stof(vec[1]);
                skylDir.y = std::stof(vec[2]);
                skylDir.z = std::stof(vec[3]);
            } else if (key == "skylColour") {
                skylColour.r = std::stof(vec[1]);
                skylColour.g = std::stof(vec[2]);
                skylColour.b = std::stof(vec[3]);
            } else throw;
            continue;
        }

        if (section == "cells") {
            const string& key = vec[0];
            if (key == "cell") {
                specVec.emplace_back();
                specVec.back().name = vec[1];
            } else if (key == "path") {
                specVec.back().path = vec[1];
            } else if (key == "xyPos") {
                specVec.back().xyPos.x = std::stoi(vec[1]);
                specVec.back().xyPos.y = std::stoi(vec[2]);
            } else if (key == "zPos") {
                specVec.back().zPos = std::stof(vec[1]);
            } else if (key == "loads") {
                for (uint i = 1; i < vec.size(); i++)
                    specVec.back().loads.emplace_back(vec[i]);
            } else throw;
            continue;
        }

        if (section == "heightlayers") {
            if (vec[0] == "layer") {
                hlVec.push_back({vec[1], {}});
            } else hlVec.back().second.emplace_back(vec[0], vec[1]);
            continue;
        }
    }

    for (const Spec& spec : specVec)
        cellMap.emplace(spec.name, Cell(spec.path, spec.name, spec.loads, spec.xyPos, spec.zPos));

    for (const pair<string, vector<pair<string, string>>>& val : hlVec)
        hlMap.insert(val);

    activate_cell("CellA");
}

float World::get_maxZ4(const string& _layer, int _x, int _y) {
    float val = -INFINITY;
    bool a=false, b=false, c=false, d=false;
    for (pair<string, string>& strPair : hlMap.at(_layer)) {
        if (a && b && c && d) break;

        const Cell& cell = cellMap.at(strPair.first);
        const HeightLayer& hl = cell.hlMap.at(strPair.second);
        glm::ivec2 xy1 = cell.xyPos * 4;
        glm::ivec2 xy2 = xy1 + glm::ivec2(cell.xySize) * 4;

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

void World::activate_cell(const string& _cell) {
    objectList.clear();
    modelList.clear();
    liquidList.clear();
    dataList.clear();
    lightVec.clear();

    const Cell& activeCell = cellMap.at(_cell);
    minPos = activeCell.pos;
    maxPos = activeCell.pos + glm::vec3(activeCell.size);
    for (const Cell::SOPair& so : activeCell.objectMap) {
        Object* ptr = so.second.get();
        objectList.push_front(ptr);
        if (ptr->type == ObjType::Model)
            modelList.push_front(static_cast<Model*>(ptr));
        else if (ptr->type == ObjType::Liquid)
            liquidList.push_front(static_cast<Liquid*>(ptr));
        else if (ptr->type == ObjType::Data)
            dataList.push_front(static_cast<Data*>(ptr));
        else if (ptr->type == ObjType::Light && lightVec.size() < 8)
            lightVec.push_back(static_cast<Light*>(ptr));
    }

    for (const string& cellName : activeCell.loads) {
        const Cell& iterCell = cellMap.at(cellName);
        minPos = glm::min(minPos, iterCell.pos);
        maxPos = glm::max(maxPos, iterCell.pos + glm::vec3(iterCell.size));
        for (const Cell::SOPair& so : iterCell.objectMap) {
            Object* ptr = so.second.get();
            objectList.push_front(ptr);
            if (ptr->type == ObjType::Model)
                modelList.push_front(static_cast<Model*>(ptr));
            else if (ptr->type == ObjType::Liquid)
                liquidList.push_front(static_cast<Liquid*>(ptr));
            else if (ptr->type == ObjType::Data)
                dataList.push_front(static_cast<Data*>(ptr));
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

    uint shadowMult = std::pow(2, vidSet().smInt.crnt("shadQuality"));
    slFb.resize(skylTexSize * shadowMult);
    slFb.dsTex.set_preset(sq::Texture::Preset::SHAD);
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
        spFbArr[i].resize(light->texSize * shadowMult);
        spFbArr[i].dsTex.set_preset(sq::Texture::Preset::SHAD);
    }
}

void World::set_player_pos(glm::uvec2 _pos) {

}

void World::tick() {
    for (pair<const string, Cell>& scPair : cellMap)
        scPair.second.tick();
}

void World::calc(double _accum) {
    for (pair<const string, Cell>& scPair : cellMap)
        scPair.second.calc(_accum);
}
