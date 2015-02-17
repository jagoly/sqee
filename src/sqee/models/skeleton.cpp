#include "app/logging.hpp"
#include "gl/maths.hpp"
#include "misc/files.hpp"

#include "models/skeleton.hpp"

using namespace sq;

Animation::Pose::Pose(uint _bCount, const float* _quatData, const float* _offsData) {
    std::memcpy(quatData, _quatData, _bCount*4*sizeof(float));
    std::memcpy(offsData, _offsData, _bCount*3*sizeof(float));
}

void Animation::load(const string& _filePath) {
    string filePath = "res/models/skeletons/" + _filePath + ".sqa";
    std::vector<std::vector<string>> fileVec(sq::get_words_from_file(filePath));

    int pNum = 0;
    std::vector<std::array<float, 4>> qVec; qVec.reserve(40);
    std::vector<std::array<float, 3>> oVec; oVec.reserve(40);
    std::vector<std::pair<uint, int>> kVec;

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
            if      (key == "bCount") bCount = std::stoi(line[1]);
            else if (key == "pCount") pCount = std::stoi(line[1]);
            else if (key == "kCount") kCount = std::stoi(line[1]);
            else throw; // invalid key
            continue;
        }

        if (section == "poses") {
            if (key == "pose") pNum++;
            else {
                float qw = std::stof(line[0]);
                float qx = std::stof(line[1]);
                float qy = std::stof(line[2]);
                float qz = std::stof(line[3]);
                float ox = std::stof(line[4]);
                float oy = std::stof(line[5]);
                float oz = std::stof(line[6]);
                qVec.push_back({qw, qx, qy, qz});
                oVec.push_back({ox, oy, oz});
            } continue;
        }

        if (section == "timeline") {
            kVec.emplace_back(std::stoi(line[0]), std::stoi(line[1]));
            continue;
        }
    }

    #ifdef SQEE_DEBUG
    if (bCount != qVec.size() / pNum)
        log_warning("bCount mismatch when loading animation from $0", _filePath);
    if (pCount != uint(pNum))
        log_warning("pCount mismatch when loading animation from $0", _filePath);
    if (kCount != kVec.size() && kfrVec.size())
        log_warning("kCount mismatch when loading animation from $0", _filePath);
    #endif

    for (int i = 0; i < pNum; i++) {
        poseVec.emplace_back(bCount, (float*)qVec.data()+i*bCount*4,
                                     (float*)oVec.data()+i*bCount*3);
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
