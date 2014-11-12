#include <iostream>
#include <fstream>

#include <models/skeleton.hpp>
#include <misc/files.hpp>

using namespace sq;

void Animation::load(const string& _filePath) {
    string filePath = SQ_MODELS "skeletons/" + _filePath + ".sqa";

    std::ifstream src;
    src.open(filePath);

#ifdef SQEE_DEBUG
    if (!src.is_open())
        std::cout << "ERROR: Couldn't open file \"" << filePath << "\"" << std::endl;
#endif

    string line;
    string section = "";

    uint pCount, kCount = 0;
    int pNum = 0;
    vector<array<float, 4>> qVec; qVec.reserve(40);
    vector<array<float, 3>> oVec; oVec.reserve(40);
    vector<pair<uint, int>> kVec;
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
    if (kCount != kVec.size() && kfrVec.size())
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

    if (kfrVec.empty()) {
        kfrVec.emplace_back(0, poseVec[0]);
    }
}


void Skeleton::setup(Animation* _anim) {
    animCrnt = _anim;
    kfrCount = animCrnt->kfrVec.size();
    poseCrnt = &animCrnt->kfrVec.front().second;
    if (kfrCount > 1) {
        span = animCrnt->kfrVec.front().first;
        poseNext = &animCrnt->kfrVec[1].second;
    }
}

void Skeleton::tick() {
    if (animNext != nullptr) {
        if (++progress == span) {
            animCrnt = animNext;
            animNext = nullptr;
            kfrCount = animCrnt->kfrVec.size();
            poseCrnt = poseNext;
            if (kfrCount > 1) {
                span = animCrnt->kfrVec[kfrInd].first;
                poseNext = &animCrnt->kfrVec[++kfrInd].second;
                progress = 0;
            }
        }
    } else {
        if (kfrCount > 1) {
            if (++progress == span) {
                if (kfrInd == kfrCount) kfrInd = 0;
                uint kfrNext = kfrInd+1;
                if (kfrNext == kfrCount) kfrNext = 0;

                span = animCrnt->kfrVec[kfrInd].first;
                poseCrnt = &animCrnt->kfrVec[kfrInd].second;
                poseNext = &animCrnt->kfrVec[kfrNext].second;

                progress = 0;
                kfrInd++;
            }
        } else {
            std::memcpy(quatData, poseCrnt->quatData, animCrnt->bCount * 4 * sizeof(float));
            std::memcpy(offsData, poseCrnt->offsData, animCrnt->bCount * 3 * sizeof(float));
        }
    }
}


void Skeleton::calc(double _accum) {
    if (kfrCount > 1) {
        const float* q1 = poseCrnt->quatData;
        const float* q2 = poseNext->quatData;
        const float* o1 = poseCrnt->offsData;
        const float* o2 = poseNext->offsData;

        float percent = (progress + _accum * 24.d) / double(span);

        for (uint i = 0; i < animCrnt->bCount; i++) {
            glm::quat quatA(q1[i*4+0], q1[i*4+1], q1[i*4+2], q1[i*4+3]);
            glm::quat quatB(q2[i*4+0], q2[i*4+1], q2[i*4+2], q2[i*4+3]);
            glm::quat quatC(glm::slerp(quatA, quatB, percent));
            quatData[i*4+0] = quatC.w;
            quatData[i*4+1] = quatC.x;
            quatData[i*4+2] = quatC.y;
            quatData[i*4+3] = quatC.z;

            glm::vec3 offsA(o1[i*3+0], o1[i*3+1], o1[i*3+2]);
            glm::vec3 offsB(o2[i*3+0], o2[i*3+1], o2[i*3+2]);
            glm::vec3 offsC(glm::mix(offsA, offsB, percent));
            offsData[i*3+0] = offsC.x;
            offsData[i*3+1] = offsC.y;
            offsData[i*3+2] = offsC.z;
        }
    }
}

void Skeleton::transition(Animation* _anim, uint _kfrInd, uint _span) {
    animNext = _anim;
    kfrInd = _kfrInd;
    poseNext = &animNext->kfrVec[kfrInd].second;
    span = _span;
    progress = 0;
    kfrCount = 2;
}
