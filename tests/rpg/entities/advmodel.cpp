#include <iostream>
#include <fstream>

#include <sqee/misc/files.hpp>

#include "advmodel.hpp"

using namespace sqt;

struct SubStr {
    SubStr(const std::string& _str) {
        a = _str.find_first_not_of(' ');
        b = _str.find_first_of(' ', a) - a;
        str = _str.substr(a, b);
    }
    SubStr(const std::string& _str, const SubStr& _prev) {
        a   = _str.find_first_not_of(' ', _prev.a + _prev.b);
        b   = _str.find_first_of(' ', a) - a;
        str = _str.substr(a, b);
    }
    uint a=0, b=0; std::string str="";
    operator float() const { return std::atof(str.c_str()); }
    explicit operator int() const { return std::atoi(str.c_str()); }
};

void AdvMesh::load(const std::string& _filePath) {
    std::string filePath = _filePath + ".sqm";

    std::ifstream src;
    std::string line;
    src.open(filePath);
    std::string section = "";

    uint vCount, fCount;
    while (std::getline(src, line)) {
        bool blank = true;
        for (const char* c = line.c_str(); *c && blank; c++) {
            blank = (*c == ' ');
        } if (blank) continue;
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

        if (section == "header") {
            SubStr key(line);
            int val = int(SubStr(line, key));
            if (key.str == "vcount") { vCount = val; continue; }
            if (key.str == "fcount") { fCount = val; continue; }
            throw; // invalid key
        }

        if (section == "vertices") {
            SubStr x(line), y(line, x), z(line, y);
            SubStr nx(line, z), ny(line, nx), nz(line, ny);
            SubStr u(line, nz), v(line, u);
            SubStr b0(line, v),  b1(line, b0), b2(line, b1), b3(line, b2);
            SubStr b4(line, b3), b5(line, b4), b6(line, b5), b7(line, b6);
            SubStr w0(line, b7), w1(line, w0), w2(line, w1), w3(line, w2);
            SubStr w4(line, w3), w5(line, w4), w6(line, w5), w7(line, w6);

            vertVec.push_back({x, y, z, nx, ny, nz, u, v,
                              {int(b0), int(b1), int(b2), int(b3),
                               int(b4), int(b5), int(b6), int(b7)},
                              {w0, w1, w2, w3, w4, w5, w6, w7}});
            continue;
        }

        if (section == "faces") {
            SubStr t(line), a(line, t), b(line, a), c(line, b);
            faceVec.emplace_back(int(t), int(a), int(b), int(c));
            continue;
        }
    }

#ifdef SQEE_DEBUG
    if (vCount != vertVec.size())
        std::cout << "WARNING: vcount and actual vertex count do not match when loading mesh from \""
                  << filePath << "\"" << std::endl;
    if (fCount != faceVec.size())
        std::cout << "WARNING: fcount and actual face count do not match when loading mesh from \""
                  << filePath << "\"" << std::endl;
#endif


    iCount = fCount * 3;

    uint indices[iCount];
    uchar texNums[fCount];
    for (uint i = 0; i < fCount; i++) {
        texNums[i] = faceVec[i].t;
        indices[i*3 +0] = faceVec[i].a;
        indices[i*3 +1] = faceVec[i].b;
        indices[i*3 +2] = faceVec[i].c;
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
    int bones[vCount * 8];
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
        for (uint j = 0; j < 8; j++) {
            bones[i*8 +j] = vertVec[i].b[j];
            weights[i*8 +j] = vertVec[i].w[j];
        }
    }


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
    gl::BufferData(gl::ARRAY_BUFFER, vCount * 8 * sizeof(int), bones, gl::STATIC_DRAW);
    gl::VertexAttribIPointer(3, 4, gl::INT, 8*sizeof(int), nullptr);
    gl::VertexAttribIPointer(4, 4, gl::INT, 8*sizeof(int), (void*)(4*sizeof(int)));

    gl::GenBuffers(1, &vboW);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboW);
    gl::BufferData(gl::ARRAY_BUFFER, vCount * 8 * sizeof(float), weights, gl::STATIC_DRAW);
    gl::VertexAttribPointer(5, 4, gl::FLOAT, false, 8*sizeof(float), nullptr);
    gl::VertexAttribPointer(6, 4, gl::FLOAT, false, 8*sizeof(float), (void*)(4*sizeof(float)));
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


void AdvSkin::load(const std::string& _filePath, sq::TexHolder& _texH) {
    Json::Value root = sq::get_json_from_file(_filePath+".json");

    uint groupCnt = root["groups"].size();
    glm::uvec3 nSize = {root["nWidth"].asUInt(), root["nHeight"].asUInt(), groupCnt};
    glm::uvec3 dSize = {root["dWidth"].asUInt(), root["dHeight"].asUInt(), groupCnt};
    glm::uvec3 sSize = {root["sWidth"].asUInt(), root["sHeight"].asUInt(), groupCnt};
    std::string sID = root["sID"].asString();

    if (!(texNorm = _texH.get(sID+"/norm"))) {
        texNorm = _texH.add(sID+"/norm");
        texNorm->create(gl::TEXTURE_2D_ARRAY, gl::RGB, gl::RGB8, sq::TexPreset::L_C);
        texNorm->resize(nSize);
    }
    if (!(texDiff = _texH.get(sID+"/diff"))) {
        texDiff = _texH.add(sID+"/diff");
        texDiff->create(gl::TEXTURE_2D_ARRAY, gl::RGB, gl::RGB8, sq::TexPreset::L_C);
        texDiff->resize(dSize);
    }
    if (!(texSpec = _texH.get(sID+"/spec"))) {
        texSpec = _texH.add(sID+"/spec");
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

void Skeleton::load(const std::string& _filePath) {
    std::string filePath = _filePath + ".sqa";

    std::ifstream src;
    std::string line;
    src.open(filePath);
    std::string section = "";

    uint pCount, kCount;
    int pNum = 0;
    std::vector<std::array<float, 4>> qVec; qVec.reserve(40);
    std::vector<std::array<float, 3>> oVec; oVec.reserve(40);
    std::vector<std::pair<uint, int>> kVec;
    while (std::getline(src, line)) {
        bool blank = true;
        for (const char* c = line.c_str(); *c && blank; c++) {
            blank = (*c == ' ');
        } if (blank) continue;
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

        if (section == "header") {
            SubStr key(line);
            int val = int(SubStr(line, key));
            if (key.str == "bcount") { bCount = val; continue; }
            if (key.str == "pcount") { pCount = val; continue; }
            if (key.str == "kcount") { kCount = val; continue; }
            throw; // invalid key
        }

        if (section == "poses") {
            if (line == "pose") { pNum++; continue; }
            SubStr qw(line), qx(line, qw), qy(line, qx), qz(line, qy);
            SubStr ox(line, qz), oy(line, ox), oz(line, oy);
            qVec.push_back({qw, qx, qy, qz});
            oVec.push_back({ox, oy, oz});
            continue;
        }

        if (section == "timeline") {
            SubStr frame(line), pose(line, frame);
            kVec.emplace_back(int(frame), int(pose));
            continue;
        }
    }

#ifdef SQEE_DEBUG
    if (bCount != qVec.size() / pNum)
        std::cout << "WARNING: bcount and actual bone count do not match when loading animation from \""
                  << filePath << "\"" << std::endl;
    if (pCount != uint(pNum))
        std::cout << "WARNING: pcount and actual pose count do not match when loading animation from \""
                  << filePath << "\"" << std::endl;
    if (kCount != kVec.size())
        std::cout << "WARNING: kcount and actual keyframe count do not match when loading animation from \""
                  << filePath << "\"" << std::endl;
#endif

    for (int i = 0; i < pNum; i++) {
        poseVec.emplace_back(bCount, (float*)qVec.data()+i*bCount*4, (float*)oVec.data()+i*bCount*3);
    }

    for (auto iter = kVec.begin(); iter != kVec.end(); iter++) {
        if (iter->second != -1) {
            kfrVec.emplace_back((iter+1)->first - iter->first, poseVec[iter->second]);
        }
    }
}


void SkeletonAnim::tick() {
    if (++keyProg == pCrnt->first) {
        if (keyInd == skeleton.kfrVec.size()) keyInd = 0;
        uint keyNext = keyInd+1;
        if (keyNext == skeleton.kfrVec.size()) keyNext = 0;
        pCrnt = &skeleton.kfrVec[keyInd];
        pNext = &skeleton.kfrVec[keyNext];
        keyProg = 0;
        keyInd++;
    }
}


void SkeletonAnim::calc(double _accum) {
    const float* q1 = pCrnt->second.quatData;
    const float* q2 = pNext->second.quatData;
    const float* o1 = pCrnt->second.offsData;
    const float* o2 = pNext->second.offsData;

    float q[skeleton.bCount * 4];
    float o[skeleton.bCount * 3];

    float percent = (keyProg + _accum) / double(pCrnt->first);

    for (uint i = 0; i < skeleton.bCount; i++) {
        glm::quat quatA(q1[i*4+0], q1[i*4+1], q1[i*4+2], q1[i*4+3]);
        glm::quat quatB(q2[i*4+0], q2[i*4+1], q2[i*4+2], q2[i*4+3]);
        glm::quat quatC(glm::slerp(quatA, quatB, percent));
        q[i*4+0] = quatC.w;
        q[i*4+1] = quatC.x;
        q[i*4+2] = quatC.y;
        q[i*4+3] = quatC.z;

        glm::vec3 offsA(o1[i*3+0], o1[i*3+1], o1[i*3+2]);
        glm::vec3 offsB(o2[i*3+0], o2[i*3+1], o2[i*3+2]);
        glm::vec3 offsC(glm::mix(offsA, offsB, percent));
        o[i*3+0] = offsC.x;
        o[i*3+1] = offsC.y;
        o[i*3+2] = offsC.z;
    }

    pose = Skeleton::Pose(skeleton.bCount, q, o);
}
