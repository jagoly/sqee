#include "model.hpp"

using namespace sqt::obj;

void Mesh::load(std::string& _filePath, int _offset) {
    offset = std::max(_offset, 0);
    anim = _offset != -1;

    if (!anim) {
        vertVec.resize(1);
        keyFrames.resize(1);
        keyTimes.resize(1);
        keyCurrent = 0;
        sq::load_ply_from_file(_filePath + ".ply", vertVec[0], faceVec);
    } else {
        Json::Value root = sq::get_json_from_file(_filePath + "/timeline.json");
        vertVec.resize(root.size());
        keyFrames.resize(vertVec.size());
        keyTimes.resize(vertVec.size());
        for (uint k = 0; k < vertVec.size(); k++) {
            faceVec.clear();
            sq::load_ply_from_file(_filePath + "/" + root[k][1].asString() + ".ply", vertVec[k], faceVec);
            keyTimes[k] = root[k][0].asUInt();
        }
    }

    uint vCount = vertVec[0].size();
    iCount = faceVec.size() * 3;

    GLuint indices[iCount];
    for (uint i = 0; i < iCount / 3; i++) {
        indices[i*3 +0] = faceVec[i].x;
        indices[i*3 +1] = faceVec[i].y;
        indices[i*3 +2] = faceVec[i].z;
    }

    gl::GenBuffers(1, &ibo);
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
    gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, iCount * sizeof(GLuint), indices, gl::STATIC_DRAW);

    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);
    gl::EnableVertexAttribArray(0);
    gl::EnableVertexAttribArray(1);
    gl::EnableVertexAttribArray(2);
    gl::EnableVertexAttribArray(3);
    if (anim) {
        gl::EnableVertexAttribArray(4);
        gl::EnableVertexAttribArray(5);
        gl::EnableVertexAttribArray(6);
        gl::EnableVertexAttribArray(7);
    }

    GLfloat points[vCount * 3];
    GLfloat normals[vCount * 3];
    GLfloat texcoords[vCount * 2];
    GLfloat tangents[vCount * 3];

    for (uint k = 0; k < keyFrames.size(); k++) {
        for (uint i = 0; i < vCount; i++) {
            points[i*3 +0] = vertVec[k][i].x;
            points[i*3 +1] = vertVec[k][i].y;
            points[i*3 +2] = vertVec[k][i].z;
            normals[i*3 +0] = vertVec[k][i].nx;
            normals[i*3 +1] = vertVec[k][i].ny;
            normals[i*3 +2] = vertVec[k][i].nz;
            texcoords[i*2 +0] = vertVec[k][i].s;
            texcoords[i*2 +1] = vertVec[k][i].t;
            tangents[i*3 +0] = vertVec[k][i].tx;
            tangents[i*3 +1] = vertVec[k][i].ty;
            tangents[i*3 +2] = vertVec[k][i].tz;
        }

        // Buffers
        gl::GenBuffers(1, &keyFrames[k].vboP);
        gl::BindBuffer(gl::ARRAY_BUFFER, keyFrames[k].vboP);
        gl::BufferData(gl::ARRAY_BUFFER, 3 * vCount * sizeof(GLfloat), points, gl::STATIC_DRAW);

        gl::GenBuffers(1, &keyFrames[k].vboN);
        gl::BindBuffer(gl::ARRAY_BUFFER, keyFrames[k].vboN);
        gl::BufferData(gl::ARRAY_BUFFER, 3 * vCount * sizeof(GLfloat), normals, gl::STATIC_DRAW);

        gl::GenBuffers(1, &keyFrames[k].vboTc);
        gl::BindBuffer(gl::ARRAY_BUFFER, keyFrames[k].vboTc);
        gl::BufferData(gl::ARRAY_BUFFER, 2 * vCount * sizeof(GLfloat), texcoords, gl::STATIC_DRAW);

        gl::GenBuffers(1, &keyFrames[k].vboT);
        gl::BindBuffer(gl::ARRAY_BUFFER, keyFrames[k].vboT);
        gl::BufferData(gl::ARRAY_BUFFER, 3 * vCount * sizeof(GLfloat), tangents, gl::STATIC_DRAW);
    }

    span = keyTimes.back();
    keyCurrent = 0;
    keyNext = 0;
    tickCurrent = _offset;
    tick();
}

Mesh::~Mesh() {
    for (KeyFrame& kf : keyFrames) {
        gl::DeleteBuffers(1, &kf.vboP);
        gl::DeleteBuffers(1, &kf.vboN);
        gl::DeleteBuffers(1, &kf.vboTc);
        gl::DeleteBuffers(1, &kf.vboT);
    }
    gl::DeleteVertexArrays(1, &vao);
}

void Mesh::tick() {
    if (anim) {
        if (tickCurrent == keyTimes[keyCurrent]) {
            keyCurrent++;
            if (keyCurrent == keyTimes.size()) keyCurrent = 0;
            keyNext = keyCurrent + 1;
            if (keyNext == keyTimes.size()) keyNext = 0;
            if (tickCurrent == span) tickCurrent = 0;
            accum = 0.d;
            frac = keyTimes[keyCurrent] - tickCurrent;
        }
        tickCurrent++;
    };

    gl::BindVertexArray(vao);

    gl::BindBuffer(gl::ARRAY_BUFFER, keyFrames[keyCurrent].vboP);
    gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 0, nullptr);
    gl::BindBuffer(gl::ARRAY_BUFFER, keyFrames[keyCurrent].vboN);
    gl::VertexAttribPointer(1, 3, gl::FLOAT, false, 0, nullptr);
    gl::BindBuffer(gl::ARRAY_BUFFER, keyFrames[keyCurrent].vboTc);
    gl::VertexAttribPointer(2, 2, gl::FLOAT, false, 0, nullptr);
    gl::BindBuffer(gl::ARRAY_BUFFER, keyFrames[keyCurrent].vboT);
    gl::VertexAttribPointer(3, 3, gl::FLOAT, false, 0, nullptr);

    if (anim) {
        gl::BindBuffer(gl::ARRAY_BUFFER, keyFrames[keyNext].vboP);
        gl::VertexAttribPointer(4, 3, gl::FLOAT, false, 0, nullptr);
        gl::BindBuffer(gl::ARRAY_BUFFER, keyFrames[keyNext].vboN);
        gl::VertexAttribPointer(5, 3, gl::FLOAT, false, 0, nullptr);
        gl::BindBuffer(gl::ARRAY_BUFFER, keyFrames[keyNext].vboTc);
        gl::VertexAttribPointer(6, 2, gl::FLOAT, false, 0, nullptr);
        gl::BindBuffer(gl::ARRAY_BUFFER, keyFrames[keyNext].vboT);
        gl::VertexAttribPointer(7, 3, gl::FLOAT, false, 0, nullptr);
    }
}


void Skin::load(std::string& _filePath, int _offset, sq::TexHolder* _texHolder) {
    Json::Value root = sq::get_json_from_file(_filePath+".json");

    alpha = root["alpha"].asBool();

    std::string name;

    // Normals
    name = "models/norm/" + root["norm"].asString() + ".png";
    if (_texHolder->has(name)) {
        texNorm = _texHolder->get(name);
    } else {
        texNorm = sq::tex2D_load_file("res/textures/" + name, gl::RGB8);
        texNorm->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
        texNorm->set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
        _texHolder->set(name, texNorm);
    }

    // Diffuse
    name = "models/diff/" + root["diff"].asString() + ".png";
    if (_texHolder->has(name)) {
        texDiff = _texHolder->get(name);
    } else {
        texDiff = sq::tex2D_load_file("res/textures/" + name, gl::RGBA8);
        texDiff->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
        texDiff->set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
        _texHolder->set(name, texDiff);
    }

    // Specular
    name = "models/spec/" + root["spec"].asString() + ".png";
    if (_texHolder->has(name)) {
        texSpec = _texHolder->get(name);
    } else {
        texSpec = sq::tex2D_load_file("res/textures/" + name, gl::RGB8);
        texSpec->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
        texSpec->set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
        _texHolder->set(name, texSpec);
    }

    // Ambient
    name = "models/ambi/" + root["ambi"].asString() + ".png";
    if (_texHolder->has(name)) {
        texAmbi = _texHolder->get(name);
    } else {
        texAmbi = sq::tex2D_load_file("res/textures/" + name, gl::R8);
        texAmbi->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
        texAmbi->set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
        _texHolder->set(name, texAmbi);
    }
}


void Model::create() {
#ifdef SQEE_DEBUG
    if (!boolMap.count("ambi")) throw;
    if (!boolMap.count("shad")) throw;
    if (!intMap.count("mOff")) throw;
    if (!intMap.count("sOff")) throw;
    if (!floatMap.count("xPos")) throw;
    if (!floatMap.count("xRot")) throw;
    if (!floatMap.count("xSca")) throw;
    if (!floatMap.count("yPos")) throw;
    if (!floatMap.count("yRot")) throw;
    if (!floatMap.count("ySca")) throw;
    if (!floatMap.count("zPos")) throw;
    if (!floatMap.count("zRot")) throw;
    if (!floatMap.count("zSca")) throw;
    if (!stringMap.count("mesh")) throw;
    if (!stringMap.count("skin")) throw;
#endif

    type = Type::Model;

    shad = boolMap["shad"];
    std::string mPath = "res/models/meshes/" + stringMap["mesh"];
    std::string sPath = "res/models/skins/" + stringMap["skin"];

    sq::SIPair mPair = {stringMap["mesh"], intMap["mOff"]};
    sq::SIPair sPair = {stringMap["skin"], intMap["sOff"]};

    if (meshH->has(mPair)) {
        mesh = meshH->get(mPair);
    } else {
        mesh = Mesh::Ptr(new Mesh());
        mesh->load(mPath, mPair.second);
        meshH->set(mPair, mesh);
    }

    if (skinH->has(sPair)) {
        skin = skinH->get(sPair);
    } else {
        skin = Skin::Ptr(new Skin());
        skin->load(sPath, sPair.second, texH);
        skinH->set(sPair, skin);
    }

    pos = {floatMap["xPos"], floatMap["yPos"], floatMap["zPos"]};
    rot = {floatMap["xRot"], floatMap["yRot"], floatMap["zRot"]};
    sca = {floatMap["xSca"], floatMap["ySca"], floatMap["zSca"]};
    modelMat = glm::translate(glm::mat4(), pos);
    modelMat = glm::rotate(modelMat, rot.z, {0, 0, 1});
    modelMat = glm::rotate(modelMat, rot.y, {0, 1, 0});
    modelMat = glm::rotate(modelMat, rot.x, {1, 0, 0});
    modelMat = glm::scale(modelMat, sca);
    if (0){//boolMap["ambi"]) {
        texAmbi = sq::tex2D_load_file("res/maps/" + mapPath + "/ao/" + uid+".png", gl::R8);
        if (texAmbi == nullptr) texAmbi = sq::tex2D_load_blank({1, 1}, gl::R8);
        texAmbi->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
        texAmbi->set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
    } else {
        texAmbi = skin->texAmbi;
    }
}
