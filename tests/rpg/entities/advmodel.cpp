#include <iostream>
#include <fstream>

#include <sqee/misc/files.hpp>

#include "advmodel.hpp"

using namespace sqt;

void AdvMesh::load(const std::string& _filePath) {
    std::ifstream src;
    std::string line;

    src.open(_filePath + ".sqm");

    uint vCount, fCount;
    std::string section = "";
    while (std::getline(src, line)) {
        bool blank = true;
        for (const char* c = line.c_str(); *c; c++) {
            if (*c != ' ') { blank = false; break; }
        }
        if (blank) continue;

        const char* c = line.c_str();

        if (*c == '#') continue;

        if (*c == '{') {
            if (!section.empty()) throw; // already in a section
            section = line.substr(2, line.find_first_of(' ', 2)).c_str();
            continue;
        }

        if (*c == '}') {
            if (section.empty()) throw; // not in a section
            section = "";
            continue;
        }

        /// Parse Sections /////////////////////

        if (section == "header") {
            std::string key = line.substr(0, 6);
            int val = std::atoi(line.substr(7, line.find_first_of(' ', 8)).c_str());
            if (key == "vcount") { vCount = val; continue; }
            if (key == "fcount") { fCount = val; continue; }
            if (key == "bcount") { bCount = val; continue; }
            if (key == "wcount") { wCount = val; continue; }
            throw; // invalid key
        }

        if (section == "bones") {
            // todo
        }

        if (section == "vertices") {
            uint xA = line.find_first_not_of(' ', 0);
            uint xB = line.find_first_of(' ', xA);
            uint yA = line.find_first_not_of(' ', xB);
            uint yB = line.find_first_of(' ', yA);
            uint zA = line.find_first_not_of(' ', yB);
            uint zB = line.find_first_of(' ', zA);
            uint nxA = line.find_first_not_of(' ', zB);
            uint nxB = line.find_first_of(' ', nxA);
            uint nyA = line.find_first_not_of(' ', nxB);
            uint nyB = line.find_first_of(' ', nyA);
            uint nzA = line.find_first_not_of(' ', nyB);
            uint nzB = line.find_first_of(' ', nzA);
            uint uA = line.find_first_not_of(' ', nzB);
            uint uB = line.find_first_of(' ', uA);
            uint vA = line.find_first_not_of(' ', uB);
            uint vB = line.find_first_of(' ', vA);
            uint bA = line.find_first_of('[', vB) + 1;
            uint bB = line.find_first_of(']', bA);
            uint wA = line.find_first_of('[', bB) + 1;
            uint wB = line.find_first_of(']', wA);

            vertVec.emplace_back();

            vertVec.back().x = std::atof(line.substr(xA, xB - xA).c_str());
            vertVec.back().y = std::atof(line.substr(yA, yB - yA).c_str());
            vertVec.back().z = std::atof(line.substr(zA, zB - zA).c_str());
            vertVec.back().nx = std::atof(line.substr(nxA, nxB - nxA).c_str());
            vertVec.back().ny = std::atof(line.substr(nyA, nyB - nyA).c_str());
            vertVec.back().nz = std::atof(line.substr(nzA, nzB - nzA).c_str());
            vertVec.back().u = std::atof(line.substr(uA, uB - uA).c_str());
            vertVec.back().v = std::atof(line.substr(vA, vB - vA).c_str());


            std::string bStr = line.substr(bA, bB - bA);
            uint B = 0;
            for (uint b = 0; b < wCount; b++) {
                uint A = bStr.find_first_not_of(' ', B);
                B = bStr.find_first_of(' ', A);
                vertVec.back().b[b] = std::atof(bStr.substr(A, B - A).c_str());
            }

            std::string wStr = line.substr(wA, wB - wA);
            B = 0;
            for (uint w = 0; w < wCount; w++) {
                uint A = wStr.find_first_not_of(' ', B);
                B = wStr.find_first_of(' ', A);
                vertVec.back().w[w] = std::atof(wStr.substr(A, B - A).c_str());
            }
        }

        if (section == "faces") {
            faceVec.emplace_back();

            uint tA = line.find_first_not_of(' ', 0);
            uint tB = line.find_first_of(' ', tA);
            uint aA = line.find_first_not_of(' ', tB);
            uint aB = line.find_first_of(' ', aA);
            uint bA = line.find_first_not_of(' ', aB);
            uint bB = line.find_first_of(' ', bA);
            uint cA = line.find_first_not_of(' ', bB);
            uint cB = line.find_first_of(' ', cA);

            faceVec.back().x = std::atoi(line.substr(aA, aB - aA).c_str());
            faceVec.back().y = std::atoi(line.substr(bA, bB - bA).c_str());
            faceVec.back().z = std::atoi(line.substr(cA, cB - cA).c_str());
            faceVec.back().w = std::atoi(line.substr(tA, tB - tA).c_str());
        }
    }


    iCount = fCount * 3;

    uint indices[iCount];
    uchar texNums[fCount];
    for (uint i = 0; i < fCount; i++) {
        indices[i*3 +0] = faceVec[i].x;
        indices[i*3 +1] = faceVec[i].y;
        indices[i*3 +2] = faceVec[i].z;
        texNums[i] = faceVec[i].w;
    }

    gl::GenBuffers(1, &ibo);
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
    gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, iCount * sizeof(uint), indices, gl::STATIC_DRAW);

    gl::GenBuffers(1, &vboTn);
    gl::BindBuffer(gl::TEXTURE_BUFFER, vboTn);
    gl::BufferData(gl::TEXTURE_BUFFER, fCount * sizeof(uchar), texNums, gl::STATIC_DRAW);

    gl::GenTextures(1, &tboTn);
    gl::BindTexture(gl::TEXTURE_BUFFER, tboTn);
    gl::TexBuffer(gl::TEXTURE_BUFFER, gl::R8UI, vboTn);

    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);
    gl::EnableVertexAttribArray(0);
    gl::EnableVertexAttribArray(1);
    gl::EnableVertexAttribArray(2);
    gl::EnableVertexAttribArray(3);
    gl::EnableVertexAttribArray(4);
    gl::EnableVertexAttribArray(5);
    gl::EnableVertexAttribArray(6);

    float points[vCount * 3];
    float normals[vCount * 3];
    float texcoords[vCount * 2];
    uint bones[vCount * 8];
    float weights[vCount * 8];

    for (uint i = 0; i < vCount; i++) {
        points[i*3 +0] = vertVec[i].x;
        points[i*3 +1] = vertVec[i].y;
        points[i*3 +2] = vertVec[i].z;
        normals[i*3 +0] = vertVec[i].nx;
        normals[i*3 +1] = vertVec[i].ny;
        normals[i*3 +2] = vertVec[i].nz;
        texcoords[i*2 +0] = vertVec[i].u;
        texcoords[i*2 +1] = vertVec[i].v;
        for (uint j = 0; j < wCount; j++) {
            bones[i*8 +j] = vertVec[i].b[j];
            weights[i*8 +j] = vertVec[i].w[j];
        }
    }

    // need to edit this a little, only buffer second half of each thing if needed

    gl::GenBuffers(1, &vboP);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboP);
    gl::BufferData(gl::ARRAY_BUFFER, 3 * vCount * sizeof(float), points, gl::STATIC_DRAW);
    gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 0, nullptr);

    gl::GenBuffers(1, &vboN);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboN);
    gl::BufferData(gl::ARRAY_BUFFER, 3 * vCount * sizeof(float), normals, gl::STATIC_DRAW);
    gl::VertexAttribPointer(1, 3, gl::FLOAT, false, 0, nullptr);

    gl::GenBuffers(1, &vboTc);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboTc);
    gl::BufferData(gl::ARRAY_BUFFER, 2 * vCount * sizeof(float), texcoords, gl::STATIC_DRAW);
    gl::VertexAttribPointer(2, 2, gl::FLOAT, false, 0, nullptr);

    gl::GenBuffers(1, &vboB);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboB);
    gl::BufferData(gl::ARRAY_BUFFER, vCount * 16 * sizeof(uint), bones, gl::STATIC_DRAW);
    gl::VertexAttribIPointer(3, 4, gl::UNSIGNED_INT, 8*sizeof(uint), nullptr);
    gl::VertexAttribIPointer(4, 4, gl::UNSIGNED_INT, 8*sizeof(uint), (void*)(8*sizeof(uint)));

    gl::GenBuffers(1, &vboW);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboW);
    gl::BufferData(gl::ARRAY_BUFFER, vCount * 16 * sizeof(float), weights, gl::STATIC_DRAW);
    gl::VertexAttribPointer(5, 4, gl::FLOAT, false, 8*sizeof(float), nullptr);
    gl::VertexAttribPointer(6, 4, gl::FLOAT, false, 8*sizeof(float), (void*)(8*sizeof(float)));
}

AdvMesh::~AdvMesh() {
    gl::DeleteBuffers(1, &ibo);
    gl::DeleteBuffers(1, &vboTn);
    gl::DeleteTextures(1, &tboTn);
    gl::DeleteVertexArrays(1, &vao);
    gl::DeleteBuffers(1, &vboP);
    gl::DeleteBuffers(1, &vboN);
    gl::DeleteBuffers(1, &vboTc);
    gl::DeleteBuffers(1, &vboB);
    gl::DeleteBuffers(1, &vboW);
}

void AdvMesh::bind_buffers() {
    gl::BindVertexArray(vao);
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
    gl::ActiveTexture(gl::TEXTURE3);
    gl::BindTexture(gl::TEXTURE_BUFFER, tboTn);
}


void AdvSkin::load(std::string& _filePath, sq::TexHolder* _texH) {
    Json::Value root = sq::get_json_from_file(_filePath+".json");

    uint groupCnt = root["groups"].size();
    glm::uvec3 nSize = {root["nWidth"].asUInt(), root["nHeight"].asUInt(), groupCnt};
    glm::uvec3 dSize = {root["dWidth"].asUInt(), root["dHeight"].asUInt(), groupCnt};
    glm::uvec3 sSize = {root["sWidth"].asUInt(), root["sHeight"].asUInt(), groupCnt};
    std::string sID = root["sID"].asString();

    if (!(texNorm = _texH->get(sID+"/norm"))) {
        texNorm = _texH->add(sID+"/norm");
        texNorm->create(gl::TEXTURE_2D_ARRAY, gl::RGB, gl::RGB8, sq::TexPreset::L_C);
        texNorm->resize(nSize);
    }
    if (!(texDiff = _texH->get(sID+"/diff"))) {
        texDiff = _texH->add(sID+"/diff");
        texDiff->create(gl::TEXTURE_2D_ARRAY, gl::RGB, gl::RGB8, sq::TexPreset::L_C);
        texDiff->resize(dSize);
    }
    if (!(texSpec = _texH->get(sID+"/spec"))) {
        texSpec = _texH->add(sID+"/spec");
        texSpec->create(gl::TEXTURE_2D_ARRAY, gl::RGB, gl::RGB8, sq::TexPreset::L_C);
        texSpec->resize(sSize);
    }

    for (uint i = 0; i < groupCnt; i++) {
        Json::Value& group = root["groups"][i];
        texNorm->buffer_file("models/norm/"+group["norm"].asString(), i);
        texDiff->buffer_file("models/diff/"+group["diff"].asString(), i);
        texSpec->buffer_file("models/spec/"+group["spec"].asString(), i);
    }
}

void AdvSkin::bind_textures() {
    texNorm->bind(gl::TEXTURE0);
    texDiff->bind(gl::TEXTURE1);
    texSpec->bind(gl::TEXTURE2);
}
