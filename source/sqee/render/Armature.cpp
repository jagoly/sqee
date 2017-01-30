#include <sqee/assert.hpp>

#include <sqee/maths/Functions.hpp>

#include <sqee/misc/Algorithms.hpp>
#include <sqee/misc/Files.hpp>

#include <sqee/render/Armature.hpp>

using namespace sq;

//void Armature::create(const string& _path) {
//    string path = "assets/armatures/" + _path + ".sqa";
//    const auto fileVec = tokenise_file(path);

//    struct BoneSpec {
//        string name; int parentIndex;
//        Vec3F head, tail; QuatF quat;
//    }; vector<BoneSpec> specVec;

//    string section = "";
//    for (const auto& linePair : fileVec) {
//        const vector<string>& line = linePair.first;
//        const uint lnum = linePair.second;
//        const string& key = line.front();

//        if (key.front() == '#') continue;
//        if (key == "{") { if (section.empty()) section = line[1];
//            else throw_error(path, lnum, "Misplaced {"); continue; }
//        if (key == "}") { if (!section.empty()) section.clear();
//            else throw_error(path, lnum, "Misplaced }"); continue; }

//        if (section == "bones") {
//            if (key == "bone") {
//                specVec.emplace_back();
//                specVec.back().name = line[1];
//                specVec.back().parentIndex = stoi(line[2]);
//            }
//            //else if (key == "head") specVec.back().head = svtofv3(line, 1);
//            //else if (key == "tail") specVec.back().tail = svtofv3(line, 1);
//            //else if (key == "quat") specVec.back().quat = svtofqt(line, 1);
//            else throw_error(path, lnum, "Invalid bone property \"%s\"", key);
//        }

//        else throw_error(path, lnum, "Invalid section \"%s\"", section);
//    }

//    for (const BoneSpec& spec : specVec) {
//         boneVector.emplace_back(spec.name, spec.parentIndex, spec.head, spec.tail, spec.quat);
//    }

//    for (const string& libName : get_files_from_dir("assets/armatures/" + _path + ".lib")) {
//        load_library(_path + ".lib/" + libName.substr(0u, libName.size() - 8u));
//    }
//}


//void Armature::load_library(const string& _path) {
//    string path = "assets/armatures/" + _path + ".sqa_lib";
//    const auto fileVec = tokenise_file(path);

//    Pose* pose = nullptr; Anim* anim = nullptr;
//    vector<ArmatureBone>::const_iterator bIter;

//    string section = "";
//    for (const auto& linePair : fileVec) {
//        const vector<string>& line = linePair.first;
//        const uint lnum = linePair.second;
//        const string& key = line.front();

//        if (key.front() == '#') continue;
//        if (key == "{") { if (section.empty()) section = line[1];
//            else throw_error(path, lnum, "Misplaced {"); continue; }
//        if (key == "}") { if (!section.empty()) section.clear();
//            else throw_error(path, lnum, "Misplaced }"); continue; }

//        if (section == "poses") {
//            if (key == "pose") {
//                poseMap.emplace(line[1], Pose());
//                pose = &poseMap.at(line[1]), pose->clear();
//                pose->reserve(boneVector.size());
//                bIter = boneVector.begin();
//            }
//            //else pose->emplace_back(&*(bIter++), svtofqt(line, 0), svtofv3(line, 4));
//        }

//        else if (section == "anims") {
//            if (key == "anim") {
//                animMap.emplace(line[1], Anim()),
//                anim = &animMap.at(line[1]), anim->clear();
//            }
//            else anim->emplace_back(poseMap.at(line[0]), stou(line[1]));
//        }

//        else throw_error(path, lnum, "Invalid section \"%s\"", section);
//    }
//}


void Armature::load_bones_from_file(const string& path)
{
    for (auto& linePair : sq::tokenise_file("assets/" + path))
    {
        SQASSERT(linePair.first.size() <= 2u, "");

        mBoneNames.push_back(linePair.first[0]);

        if (linePair.first.size() == 2u)
        {
            const auto iter = algo::find(mBoneNames, linePair.first[1]);
            SQASSERT(iter != mBoneNames.end(), "parent not found");
            mBoneParents.push_back(std::distance(mBoneNames.begin(), iter));
        }
        else mBoneParents.push_back(-1);
    }
}

void Armature::refresh_matrices()
{
    const uint boneCount = mRestPose.size();

    mBaseMats.reserve(boneCount);
    mInverseMats.reserve(boneCount);

    for (uint i = 0u; i < boneCount; ++i)
    {
        const PoseBone& bone = mRestPose[i];
        const int32_t parentIndex = mBoneParents[i];

        mBaseMats.push_back(maths::transform(bone.offset, bone.rotation, bone.scale));

        if (parentIndex >= 0)
            mBaseMats.back() = mBaseMats[uint(parentIndex)] * mBaseMats.back();

        mInverseMats.push_back(maths::affine_inverse(mBaseMats.back()));
    }
}

Armature::Pose Armature::load_pose_from_file(const string& path) const
{
    const uint boneCount = mBoneNames.size();

    const auto lines = sq::tokenise_file("assets/" + path);
    SQASSERT(boneCount == lines.size(), "bone count mismatch");

    Armature::Pose result;
    result.reserve(mBoneNames.size());

    for (auto& linePair : lines)
    {
        const auto& line = linePair.first;
        SQASSERT(line.size() == 10u, "invalid pose bone");

        result.emplace_back();

        result.back().offset = { stof(line[0]), stof(line[1]), stof(line[2]) };
        result.back().rotation = { stof(line[3]), stof(line[4]), stof(line[5]), stof(line[6]) };
        result.back().scale = { stof(line[7]), stof(line[8]), stof(line[9]) };
    }

    return result;
}

Armature::Pose Armature::blend_poses(const Pose& poseA, const Pose& poseB, float factor) const
{
    const uint boneCount = mRestPose.size();

    SQASSERT(boneCount == poseA.size(), "bone count mismatch");
    SQASSERT(boneCount == poseB.size(), "bone count mismatch");

    Armature::Pose result;
    result.reserve(boneCount);

    for (uint i = 0u; i < boneCount; ++i)
    {
        const PoseBone& a = poseA[i];
        const PoseBone& b = poseB[i];
        result.emplace_back();

        result.back().offset = maths::mix(a.offset, b.offset, factor);
        result.back().rotation = maths::slerp(a.rotation, b.rotation, factor);
        result.back().scale = maths::mix(a.scale, b.scale, factor);
    }

    return result;
}

vector<Mat34F> Armature::pose_to_ubo_data(const Pose& pose) const
{
    const uint boneCount = mBoneNames.size();

    SQASSERT(boneCount == pose.size(), "bone count mismatch");

    vector<Mat4F> localMats;
    vector<Mat4F> finalMats;

    for (uint i = 0u; i < boneCount; ++i)
    {
        const PoseBone& bone = pose[i];
        const int32_t parentIndex = mBoneParents[i];

        localMats.push_back(maths::transform(bone.offset, bone.rotation, bone.scale));

        if (parentIndex >= 0)
        {
            localMats.back() = mBaseMats[uint(parentIndex)] * localMats.back() * mInverseMats[i];
            finalMats.push_back(finalMats[uint(parentIndex)] * localMats.back());
        }
        else
        {
            localMats.back() = localMats.back() * mInverseMats[i];
            finalMats.push_back(localMats.back());
        }
    }

    vector<Mat34F> result;

    for (const Mat4F& mat : finalMats)
    {
        result.push_back(Mat34F(maths::transpose(mat)));
    }

    return result;
}
