#include <glm/mat3x4.hpp>
#include <glm/matrix.hpp>

#include <sqee/app/logging.hpp>
#include <sqee/misc/files.hpp>
#include <sqee/render/animation.hpp>

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
                Bone bone { {stof(line[3]), stof(line[0]), stof(line[1]), stof(line[2])},
                            {stof(line[4]), stof(line[5]), stof(line[6])} };
                poseVec.back().emplace_back(bone);
            } continue;
        }

        if (section == "timelines") {
            if (key == "timeline") timelineVec.emplace_back();
            else timelineVec.back().emplace_back(poseVec[stou(line[0])], stou(line[1]));
            continue;
        }
    }
}

glm::mat3x4 Animation::to_matrix(Bone _bone) {
    fmat3 m = glm::mat3_cast(_bone.quat);
    return glm::mat3x4(
        m[0].x, m[1].x, m[2].x, _bone.offs.x,
        m[0].y, m[1].y, m[2].y, _bone.offs.y,
        m[0].z, m[1].z, m[2].z, _bone.offs.z
    );
}


ResHolder<Animation>& sq::res::anim() {
    static ResHolder<Animation> holder;
    return holder;
}
string& sq::res::anim_path() {
    static string path;
    return path;
}
