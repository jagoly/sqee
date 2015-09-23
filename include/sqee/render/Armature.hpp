#pragma once
#include <sqee/forward.hpp>

#include <sqee/misc/ResHolder.hpp>

namespace sq {

/// An armature bone in it's resting state
struct ArmatureBone {
    ArmatureBone(string _name, int _prntInd, fvec3 _head, fvec3 _tail, fquat _quat) :
        name(_name), prntInd(_prntInd), head(_head), tail(_tail), quat(_quat) {}
    string name; int prntInd; fvec3 head, tail; fquat quat;
};

/// A transformation of an armature bone
struct ArmaTransform {
    ArmaTransform(const ArmatureBone* _bone, fquat _rotation, fvec3 _offset) :
        bone(_bone), rotation(_rotation), offset(_offset) {}
    const ArmatureBone* bone; fquat rotation; fvec3 offset;
};

/// The SQEE Armature class
class Armature : NonCopyable {
public:
    void create(const string& _path);
    void load_library(const string& _path);

    vector<ArmatureBone> boneVector;
    using Pose = vector<ArmaTransform>;
    using Anim = vector<pair<Pose&, uint>>;
    unordered_map<string, Pose> poseMap;
    unordered_map<string, Anim> animMap;

    using UboData = vector<glm::fmat3x4>;
    static UboData make_UboData(Pose& _pose);
    static Pose mix_Poses(const Pose& _a, const Pose& _b, float _factor);
};


namespace res {
ResHolder<Armature>& arma();
}

}
