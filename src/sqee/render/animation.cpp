#include "sqee/app/logging.hpp"
#include "sqee/misc/files.hpp"
#include "sqee/render/animation.hpp"

using namespace sq;

void Animation::create(const string& _path) {
    string path = res::anim_path() + _path + ".sqa";
    vector<vector<string>> fileVec(sq::get_words_from_file(path));

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
            if      (key == "bCount") bCount = stou(line[1]);
            else if (key == "pCount") pCount = stou(line[1]);
            else if (key == "tCount") tCount = stou(line[1]);
            else throw;
            continue;
        }

        if (section == "poses") {
            if (key == "pose") poseVec.emplace_back();
            else {
                poseVec.back().first.emplace_back(stof(line[0]), stof(line[1]), stof(line[2]), stof(line[3]));
                poseVec.back().second.emplace_back(stof(line[4]), stof(line[5]), stof(line[6]));
            } continue;
        }

        if (section == "timelines") {
            if (key == "timeline") timelineVec.emplace_back();
            else timelineVec.back().emplace_back(poseVec[stou(line[0])], stou(line[1]));
            continue;
        }
    }
}


ResHolder<Animation>& sq::res::anim() {
    static ResHolder<Animation> holder;
    return holder;
}
string& sq::res::anim_path() {
    static string path;
    return path;
}
