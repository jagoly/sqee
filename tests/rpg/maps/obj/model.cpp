#include <iostream>
#include <fstream>

#include "model.hpp"

using namespace sqt::obj;

void Mesh::load(const std::string& _filePath) {
    /// Load File ////////////////////
    // This sucks ass. Fix it.

    std::ifstream src;
    std::string line;

    src.open(_filePath + ".ply");

    std::getline(src, line);
    if (line != "ply") throw; // First line incorrect
    std::getline(src, line);
    if (line != "format ascii 1.0") throw; // Second line incorrect
    std::getline(src, line);
    if (line.substr(0, 7) != "comment") throw; // Third line must be comment
    std::getline(src, line);
    if (line.substr(0, 6) != "bounds") throw; // Third line must be bounding box
    {
    float vals[8];
    uint A=0, B=0;
    for (uint j = 0; j < 6; j++) {
        A = line.find_first_not_of(' ', B);
        B = line.find_first_of(' ', A);
        vals[j] = std::atof(line.substr(A, B - A).c_str());
    }
    bBox = sq::Box(vals);
    }
    std::getline(src, line);
    if (line.substr(0, 14) != "element vertex") throw; // No vertex count
    uint vCount = std::atoi(line.substr(15).c_str());

    std::getline(src, line);
    if (line != "property float x") throw; // Bad Property
    std::getline(src, line);
    if (line != "property float y") throw; // Bad Property
    std::getline(src, line);
    if (line != "property float z") throw; // Bad Property
    std::getline(src, line);
    if (line != "property float nx") throw; // Bad Property
    std::getline(src, line);
    if (line != "property float ny") throw; // Bad Property
    std::getline(src, line);
    if (line != "property float nz") throw; // Bad Property
    std::getline(src, line);
    if (line != "property float s") throw; // Bad Property
    std::getline(src, line);
    if (line != "property float t") throw; // Bad Property

    std::getline(src, line);
    if (line.substr(0, 12) != "element face") throw; // No Face Count
    uint fCount = std::atoi(line.substr(13).c_str());

    std::getline(src, line);
    if (line != "property list uchar uint vertex_indices") throw; // Bad Face Format
    std::getline(src, line);
    if (line != "end_header") throw; // No end of header

    for (uint i = 0; i < vCount; i++) {
        std::getline(src, line);
        float vals[8];
        uint A=0, B=0;
        for (uint j = 0; j < 8; j++) {
            A = line.find_first_not_of(' ', B);
            B = line.find_first_of(' ', A);
            vals[j] = std::atof(line.substr(A, B - A).c_str());
        }
        vertVec.emplace_back(vals);
    }

    for (uint i = 0; i < fCount; i++) {
        std::getline(src, line);
        if (line.substr(0, 1) != "3") throw; // Only tris are supported
        line = line.substr(2);
        uint vals[3];
        uint A=0, B=0;
        for (uint j = 0; j < 3; j++) {
            A = line.find_first_not_of(' ', B);
            B = line.find_first_of(' ', A);
            vals[j] = std::atoi(line.substr(A, B - A).c_str());
        }
        faceVec.emplace_back(vals[0], vals[1], vals[2]);
    }
    //////////////////////////

    iCount = faceVec.size() * 3;

    GLuint indices[iCount];
    for (uint i = 0; i < faceVec.size(); i++) {
        indices[i*3 +0] = faceVec[i].x;
        indices[i*3 +1] = faceVec[i].y;
        indices[i*3 +2] = faceVec[i].z;
    }

    gl::GenBuffers(1, &ibo);
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
    gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, iCount * sizeof(uint), indices, gl::STATIC_DRAW);

    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);
    gl::EnableVertexAttribArray(0);
    gl::EnableVertexAttribArray(1);
    gl::EnableVertexAttribArray(2);

    float points[vCount * 3];
    float normals[vCount * 3];
    float texcoords[vCount * 2];

    for (uint i = 0; i < vCount; i++) {
        points[i*3 +0] = vertVec[i].x;
        points[i*3 +1] = vertVec[i].y;
        points[i*3 +2] = vertVec[i].z;
        normals[i*3 +0] = vertVec[i].nx;
        normals[i*3 +1] = vertVec[i].ny;
        normals[i*3 +2] = vertVec[i].nz;
        texcoords[i*2 +0] = vertVec[i].u;
        texcoords[i*2 +1] = vertVec[i].v;
    }

    // !!!!!!!!!!!!!!! //
    // need to edit this a little, only buffer second half of each thing if needed

    // Buffers
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
}

Mesh::~Mesh() {
    gl::DeleteBuffers(1, &ibo);
    gl::DeleteVertexArrays(1, &vao);
    gl::DeleteBuffers(1, &vboP);
    gl::DeleteBuffers(1, &vboN);
    gl::DeleteBuffers(1, &vboTc);
}

void Mesh::bind_buffers() {
    gl::BindVertexArray(vao);
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
}


void Skin::load(const std::string& _filePath, sq::TexHolder* _texH) {
    Json::Value root = sq::get_json_from_file(_filePath+".json");

    alpha = root["alpha"].asBool();

    std::string name;

    name = "models/norm/"+root["norm"].asString();
    if (!(texNorm = _texH->get(name))) {
        texNorm = _texH->add(name);
        texNorm->create(gl::TEXTURE_2D, gl::RGB, gl::RGB8, name, sq::TexPreset::L_C);
    }

    name = "models/diff/"+root["diff"].asString();
    if (!(texDiff = _texH->get(name))) {
        texDiff = _texH->add(name);
        texDiff->create(gl::TEXTURE_2D, gl::RGBA, gl::RGBA8, name, sq::TexPreset::L_C);
    }

    name = "models/spec/"+root["spec"].asString();
    if (!(texSpec = _texH->get(name))) {
        texSpec = _texH->add(name);
        texSpec->create(gl::TEXTURE_2D, gl::RGB, gl::RGB8, name, sq::TexPreset::L_C);
    }

    name = "models/ambi/"+root["ambi"].asString();
    if (!(texAmbi = _texH->get(name))) {
        texAmbi = _texH->add(name);
        texAmbi->create(gl::TEXTURE_2D, gl::RED, gl::R8, name, sq::TexPreset::L_C);
    }
}

void Skin::bind_textures() {
    texNorm->bind(gl::TEXTURE0);
    texDiff->bind(gl::TEXTURE1);
    texSpec->bind(gl::TEXTURE2);
    texAmbi->bind(gl::TEXTURE3);
}


void Model::create() {
#ifdef SQEE_DEBUG
    SQ_BOOLCHECK("ambi")
    SQ_BOOLCHECK("shad")
    SQ_BOOLCHECK("refl")
    SQ_FLOATCHECK("xPos")
    SQ_FLOATCHECK("xRot")
    SQ_FLOATCHECK("xSca")
    SQ_FLOATCHECK("yPos")
    SQ_FLOATCHECK("yRot")
    SQ_FLOATCHECK("ySca")
    SQ_FLOATCHECK("zPos")
    SQ_FLOATCHECK("zRot")
    SQ_FLOATCHECK("zSca")
    SQ_STRINGCHECK("mesh")
    SQ_STRINGCHECK("skin")
#endif

    type = Type::Model;

    shad = boolMap["shad"];
    refl = boolMap["refl"];
    std::string mPath = "res/models/meshes/" + stringMap["mesh"];
    std::string sPath = "res/models/skins/" + stringMap["skin"];

    std::string mName = stringMap["mesh"];
    std::string sName = stringMap["skin"];

    if (!(mesh = meshH->get(mName))) {
        mesh = meshH->add(mName);
        mesh->load(mPath);
    }

    if (!(skin = skinH->get(sName))) {
        skin = skinH->add(sName);
        skin->load(sPath, texH);
    }

    pos = {floatMap["xPos"], floatMap["yPos"], floatMap["zPos"]};
    rot = {floatMap["xRot"], floatMap["yRot"], floatMap["zRot"]};
    sca = {floatMap["xSca"], floatMap["ySca"], floatMap["zSca"]};
    modelMat = glm::translate(glm::mat4(), pos);
    modelMat = glm::rotate(modelMat, rot.z, {0, 0, 1});
    modelMat = glm::rotate(modelMat, rot.y, {0, 1, 0});
    modelMat = glm::rotate(modelMat, rot.x, {1, 0, 0});
    modelMat = glm::scale(modelMat, sca);

    bBox.init(mesh->bBox);
    bBox.translate(pos);
    // need to add rotate and scale
    bBox.recalc();

    // need to fix individual object ambient light
    texAmbi = skin->texAmbi;
}
