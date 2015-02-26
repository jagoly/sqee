#include <cstring>

#include "app/logging.hpp"
#include "misc/files.hpp"

#include "models/animation.hpp"

using namespace sq;

Animation::Pose::Pose(uint _bCount, const float* _quatData, const float* _offsData) {
    std::memcpy(quatData, _quatData, _bCount*4*sizeof(float));
    std::memcpy(offsData, _offsData, _bCount*3*sizeof(float));
}

void Animation::create(const string& _filePath) {
    string filePath = "res/models/skeletons/" + _filePath + ".sqa";
    std::vector<std::vector<string>> fileVec(sq::get_words_from_file(filePath));

    int pNum = 0;
    std::vector<std::array<float, 4>> qVec; qVec.reserve(40);
    std::vector<std::array<float, 3>> oVec; oVec.reserve(40);
    std::vector<std::pair<uint, int>> kVec;

    string section = "";
    for (const auto& line : fileVec) {
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


ResHolder<Animation>& sq::res::animation() {
    static ResHolder<Animation> holder;
    return holder;
}
