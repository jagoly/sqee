#include <sqee/redist/tinyformat.hpp>
#include <sqee/render/Armature.hpp>
#include <sqee/misc/Files.hpp>

using namespace sq;

template<typename... Args>
void throw_error(const string& _path, int _lnum, const string& _msg, const Args&... _args) {
    string message = tfm::format("Parsing SQA \"%s\"\nline %d: ", _path, _lnum);
    throw runtime_error(message + tfm::format(_msg.c_str(), _args...));
}


void Armature::create(const string& _path) {
    string path = res::path() + "armatures/" + _path + ".sqa";
    const auto fileVec = tokenise_file(path);

    struct BoneSpec {
        string name; int parentIndex;
        fvec3 head, tail; fquat quat;
    }; vector<BoneSpec> specVec;

    string section = "";
    for (const auto& linePair : fileVec) {
        const vector<string>& line = linePair.first;
        const uint lnum = linePair.second;
        const string& key = line.front();

        if (key.front() == '#') continue;
        if (key == "{") { if (section.empty()) section = line[1];
            else throw_error(path, lnum, "Misplaced {"); continue; }
        if (key == "}") { if (!section.empty()) section.clear();
            else throw_error(path, lnum, "Misplaced }"); continue; }

        if (section == "bones") {
            if (key == "bone") {
                specVec.emplace_back();
                specVec.back().name = line[1];
                specVec.back().parentIndex = stoi(line[2]);
            }
            else if (key == "head") specVec.back().head = svtofv3(line, 1);
            else if (key == "tail") specVec.back().tail = svtofv3(line, 1);
            else if (key == "quat") specVec.back().quat = svtofqt(line, 1);
            else throw_error(path, lnum, "Invalid bone property \"%s\"", key);
        }

        else throw_error(path, lnum, "Invalid section \"%s\"", section);
    }

    for (const BoneSpec& spec : specVec) {
         boneVector.emplace_back(spec.name, spec.parentIndex, spec.head, spec.tail, spec.quat);
    }

    for (const string& libName : get_files_from_dir(res::path() + "armatures/" + _path + ".lib")) {
        load_library(_path + ".lib/" + libName.substr(0u, libName.size() - 8u));
    }
}


void Armature::load_library(const string& _path) {
    string path = res::path() + "armatures/" + _path + ".sqa_lib";
    const auto fileVec = tokenise_file(path);

    Pose* pose = nullptr; Anim* anim = nullptr;
    vector<ArmatureBone>::const_iterator bIter;

    string section = "";
    for (const auto& linePair : fileVec) {
        const vector<string>& line = linePair.first;
        const uint lnum = linePair.second;
        const string& key = line.front();

        if (key.front() == '#') continue;
        if (key == "{") { if (section.empty()) section = line[1];
            else throw_error(path, lnum, "Misplaced {"); continue; }
        if (key == "}") { if (!section.empty()) section.clear();
            else throw_error(path, lnum, "Misplaced }"); continue; }

        if (section == "poses") {
            if (key == "pose") {
                poseMap.emplace(line[1], Pose());
                pose = &poseMap.at(line[1]), pose->clear();
                pose->reserve(boneVector.size());
                bIter = boneVector.begin();
            }
            else pose->emplace_back(&*(bIter++), svtofqt(line, 0), svtofv3(line, 4));
        }

        else if (section == "anims") {
            if (key == "anim") {
                animMap.emplace(line[1], Anim()),
                anim = &animMap.at(line[1]), anim->clear();
            }
            else anim->emplace_back(poseMap.at(line[0]), stou(line[1]));
        }

        else throw_error(path, lnum, "Invalid section \"%s\"", section);
    }
}


Armature::UboData Armature::make_UboData(Pose& _pose) {
    vector<pair<fquat, fvec3>> cacheVector(_pose.size());
    UboData retUboData; fvec3 translation;

    for (uint tfInd = 0u; tfInd != _pose.size(); tfInd++) {
        fquat& crntRotation = cacheVector[tfInd].first;
        fvec3& crntPosition = cacheVector[tfInd].second;
        const ArmaTransform& tf = _pose[tfInd];

        if (tf.bone->prntInd != -1) {

            fquat prntRotation = cacheVector[tf.bone->prntInd].first;
            fvec3 prntPosition = cacheVector[tf.bone->prntInd].second;
            const ArmaTransform& parent = _pose[tf.bone->prntInd];

            crntRotation = prntRotation * tf.bone->quat
                         * tf.rotation * glm::inverse(tf.bone->quat);

            crntPosition = prntPosition + prntRotation
                         * (parent.bone->tail - parent.bone->head);

            translation = crntPosition - crntRotation * tf.bone->head;

        } else {
            crntRotation = tf.bone->quat * tf.rotation * glm::inverse(tf.bone->quat);
            crntPosition = tf.bone->head + tf.offset; translation = tf.offset;
        }

        //fvec4 vquat = fvec4(crntRotation.w, crntRotation.x, crntRotation.y, crntRotation.z);
        //sq::log_only("%s: %s | %s", tf.bone->name, glm::to_string(vquat), glm::to_string(translation));

        fmat3 mat = glm::mat3_cast(crntRotation);
        retUboData.emplace_back(
            mat[0].x, mat[1].x, mat[2].x, translation.x,
            mat[0].y, mat[1].y, mat[2].y, translation.y,
            mat[0].z, mat[1].z, mat[2].z, translation.z
        );
    }

    return retUboData;
}

Armature::Pose Armature::mix_Poses(const Pose& _a, const Pose& _b, float _factor) {
    Pose retPose; retPose.reserve(_a.size());
    for (uint tfInd = 0u; tfInd < _a.size(); tfInd++) {
        retPose.emplace_back(_a[tfInd].bone,
            glm::slerp(_a[tfInd].rotation, _b[tfInd].rotation, _factor),
            glm::mix(_a[tfInd].offset, _b[tfInd].offset, _factor));
    } return retPose;
}


ResHolder<Armature>& sq::res::arma() {
    static ResHolder<Armature> holder;
    return holder;
}
