#pragma once

#include "Object.hpp"
#include "Animation.hpp"

namespace sqt { namespace wcoe {

class ModelSkelly : public Object {
public:
    ModelSkelly(const string& _name, Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void refresh(); void tick();
    void calc(double _accum);

    bool   PROP_render   = false;
    bool   PROP_shadow   = false;
    bool   PROP_decals   = false;
    fvec3  PROP_position = {0.f, 0.f, 0.f};
    fquat  PROP_rotation = {1.f, 0.f, 0.f, 0.f};
    fvec3  PROP_scale    = {1.f, 1.f, 1.f};
    string PROP_arma     = "";
    string PROP_mesh     = "";
    string PROP_skin     = "";
    string PROP_pose     = "";
    string PROP_anim     = "";

    AnimatorFVec3 ANIM_position {&PROP_position};
    AnimatorFQuat ANIM_rotation {&PROP_rotation};
    AnimatorFVec3 ANIM_scale    {&PROP_scale};
    void animate();

    void FUNC_stop(uint _time);
    void FUNC_loop(uint _time);
    void FUNC_play(uint _timeA, uint _timeB);

    unique_ptr<sq::UniformBuffer> ubo;
    sq::Armature* arma = nullptr;
    sq::Mesh* mesh = nullptr;
    sq::Skin* skin = nullptr;
    sq::Sphere sphere;
    bool negScale;
    fmat4 matrix;

private:
    uint index = 0u, ticks = 0u;
    uint span = 0u, spanEnd = 0u;
    vector<pair<vector<sq::ArmatureTrnsfrm>&, uint>> anim;
    vector<sq::ArmatureTrnsfrm> poseCalc, poseCrnt, poseNext;
    enum class State { Running, Ending, Paused, Done };
    State state = State::Done; bool looping = false;
};

template<> struct ObjTraits<ModelSkelly> {
    static constexpr ObjType type() { return ObjType::ModelSkelly; }
};

}}
