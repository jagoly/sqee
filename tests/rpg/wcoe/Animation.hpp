#pragma once
#include <sqee/forward.hpp>
#include <deque>

namespace sqt { namespace wcoe {

namespace priv {
inline float con_float(float _a, float _b) { return _a + _b; }
inline float mix_float(float _a, float _b, float _m) { return glm::mix(_a, _b, _m); }
inline fvec2 con_fvec2(fvec2 _a, fvec2 _b) { return _a + _b; }
inline fvec2 mix_fvec2(fvec2 _a, fvec2 _b, float _m) { return glm::mix(_a, _b, _m); }
inline fvec3 con_fvec3(fvec3 _a, fvec3 _b) { return _a + _b; }
inline fvec3 mix_fvec3(fvec3 _a, fvec3 _b, float _m) { return glm::mix(_a, _b, _m); }
inline fvec4 con_fvec4(fvec4 _a, fvec4 _b) { return _a + _b; }
inline fvec4 mix_fvec4(fvec4 _a, fvec4 _b, float _m) { return glm::mix(_a, _b, _m); }
inline fquat con_fquat(fquat _a, fquat _b) { return _a * _b; }
inline fquat mix_fquat(fquat _a, fquat _b, float _m) { return glm::slerp(_a, _b, _m); }
}

template<class T, T(*FCon)(T, T), T(*FMix)(T, T, float), bool Normalize>
class Animator : NonCopyable {
public:
    Animator(T* _property) : property(_property) {}
    bool active() { return running; }
    T* const property;

    vector<T> values;
    vector<uint> times;

    void start() {
        nextVal = *property;
        timeLeft = 0u; running = true;
        valueDeq.clear(); timeDeq.clear();
        for (const auto& value : values) valueDeq.emplace_back(value);
        for (const auto& time : times) timeDeq.emplace_back(time);
    }

    bool tick() {
        if (timeLeft <= 1u) {
            if (valueDeq.empty() || timeDeq.empty()) {
                running = false; *property = nextVal; return true; }
            crntVal = nextVal; nextVal = FCon(crntVal, valueDeq.front());
            timeTotal = timeDeq.front(); timeLeft = timeTotal;
            valueDeq.pop_front(); timeDeq.pop_front();
        } else timeLeft--; return false;
    }

    void calc(double _accum) {
        float percent = (timeTotal - timeLeft + _accum*24.0) / timeTotal;
        *property = FMix(crntVal, nextVal, glm::min(percent, 1.f));
        if (Normalize) *property = glm::normalize(*property);
    }

private:
    std::deque<T> valueDeq;
    std::deque<uint> timeDeq;
    bool running = false;
    uint timeTotal = 0u;
    uint timeLeft = 0u;
    T crntVal, nextVal;
};

using AnimatorFloat = Animator<float, priv::con_float, priv::mix_float, false>;
using AnimatorFVec2 = Animator<fvec2, priv::con_fvec2, priv::mix_fvec2, false>;
using AnimatorFVec3 = Animator<fvec3, priv::con_fvec3, priv::mix_fvec3, false>;
using AnimatorFVec4 = Animator<fvec4, priv::con_fvec4, priv::mix_fvec4, false>;
using AnimatorFNorm = Animator<fvec3, priv::con_fvec3, priv::mix_fvec3, true>;
using AnimatorFQuat = Animator<fquat, priv::con_fquat, priv::mix_fquat, true>;

}}
