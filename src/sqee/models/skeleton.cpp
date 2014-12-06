#include <fstream>
#include <sstream>

#include <models/skeleton.hpp>
#include <misc/files.hpp>

using namespace sq;

void Animation::load(const string& _filePath) {
    string filePath = SQ_MODELS "skeletons/" + _filePath + ".sqa";
    std::ifstream src(filePath);

    if (!src.is_open()) cout << "ERROR: Couldn't open file \"" << filePath << "\"" << endl;

    int pNum = 0;
    vector<array<float, 4>> qVec; qVec.reserve(40);
    vector<array<float, 3>> oVec; oVec.reserve(40);
    vector<pair<uint, int>> kVec;

    string line, section = "";
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
            string& key = vec[0];
            uint val = std::stoi(vec[1]);
            if (key == "bCount") { bCount = val; continue; }
            if (key == "pCount") { pCount = val; continue; }
            if (key == "kCount") { kCount = val; continue; }
            throw; // invalid key
        }

        if (section == "poses") {
            if (vec[0] == "pose") { pNum++; continue; }

            float qw = std::stof(vec[0]);
            float qx = std::stof(vec[1]);
            float qy = std::stof(vec[2]);
            float qz = std::stof(vec[3]);
            float ox = std::stof(vec[4]);
            float oy = std::stof(vec[5]);
            float oz = std::stof(vec[6]);

            qVec.push_back({qw, qx, qy, qz});
            oVec.push_back({ox, oy, oz});
            continue;
        }

        if (section == "timeline") {
            uint frame = std::stoi(vec[0]);
            int pose = std::stoi(vec[1]);

            kVec.emplace_back(frame, pose);
            continue;
        }
    }

    #ifdef SQEE_DEBUG
    if (bCount != qVec.size() / pNum)
        std::cout << "WARNING: bCount mismatch when loading animation from \""
                  << filePath << "\"" << std::endl;
    if (pCount != uint(pNum))
        std::cout << "WARNING: pCount mismatch when loading animation from \""
                  << filePath << "\"" << std::endl;
    if (kCount != kVec.size() && kfrVec.size())
        std::cout << "WARNING: kCount mismatch when loading animation from \""
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
