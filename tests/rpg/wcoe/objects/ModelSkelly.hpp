#pragma once

#include <sqee/forward.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Quaternion.hpp>
#include <sqee/maths/Volumes.hpp>

#include "../Object.hpp"
#include "../Animation.hpp"

namespace sqt { namespace wcoe {

class ModelSkelly : public Object {
public:
    ModelSkelly(const string& _name, Cell* _cell);

    void load_from_spec(const ObjSpec& _spec);

    void refresh(), update();
    void calc(double _accum);
    void animate();

    bool   PROP_shadow   = false;
    bool   PROP_decals   = false;
    bool   PROP_probes   = false;
    Vec3F  PROP_position = {0.f, 0.f, 0.f};
    QuatF  PROP_rotation = {0.f, 0.f, 0.f, 1.f};
    Vec3F  PROP_scale    = {1.f, 1.f, 1.f};
    string PROP_mesh     = "";
    string PROP_skin     = "";
    string PROP_arma     = "";
    string PROP_pose     = "";
    string PROP_anim     = "";

    AnimatorVec3F ANIM_position {&PROP_position};
    AnimatorQuatF ANIM_rotation {&PROP_rotation};
    AnimatorVec3F ANIM_scale    {&PROP_scale};

    void FUNC_stop(uint _time);
    void FUNC_loop(uint _time);
    void FUNC_play(uint _timeA, uint _timeB);

    sq::Armature* arma = nullptr;
    sq::Mesh* mesh = nullptr;
    sq::Skin* skin = nullptr;
    sq::UniformBuffer ubo;
    sq::Sphere sphere;
    bool negScale;
    Mat4F matrix;

private:
    uint index = 0u, ticks = 0u;
    uint span = 0u, spanEnd = 0u;
    vector<pair<vector<sq::ArmaTransform>&, uint>> anim;
    vector<sq::ArmaTransform> poseCalc, poseCrnt, poseNext;
    enum class State { Running, Ending, Paused, Done };
    State state = State::Done; bool looping = false;
};

}}
