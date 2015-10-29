#pragma once

#include <unordered_map>

#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>

namespace sq {

/// An armature bone in it's resting state
struct ArmatureBone {
    ArmatureBone(const string& _name, int _prntInd, Vec3F _head, Vec3F _tail, QuatF _quat)
        : name(_name), prntInd(_prntInd), head(_head), tail(_tail), quat(_quat) {}
    string name; int prntInd; Vec3F head, tail; QuatF quat;
};

/// A transformation of an armature bone
struct ArmaTransform {
    ArmaTransform(const ArmatureBone* _bone, QuatF _rotation, Vec3F _offset)
        : bone(_bone), rotation(_rotation), offset(_offset) {}
    const ArmatureBone* bone; QuatF rotation; Vec3F offset;
};

/// The SQEE Armature class
class Armature : NonCopyable {
public:
    void create(const string& _path);
    void load_library(const string& _path);

    vector<ArmatureBone> boneVector;
    using Pose = vector<ArmaTransform>;
    using Anim = vector<pair<Pose&, uint>>;
    std::unordered_map<string, Pose> poseMap;
    std::unordered_map<string, Anim> animMap;

    using UboData = vector<Mat34F>;
    static UboData make_UboData(Pose& _pose);
    static Pose mix_Poses(const Pose& _a, const Pose& _b, float _factor);
};

}
