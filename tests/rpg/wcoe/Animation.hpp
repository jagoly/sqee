#pragma once

#include <deque>
#include <vector>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>

namespace sqt { namespace wcoe {

template<class T, T(*FCon)(T, T), T(*FMix)(T, T, float), T(*FNorm)(T) = nullptr>
class Animator : sq::NonCopyable {
public:
    Animator(T* _property) : property(_property) {}
    bool active() const { return running; }
    T* const property;

    std::vector<T> values;
    std::vector<uint> times;

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
        *property = FMix(crntVal, nextVal, std::min(percent, 1.f));
        if (FNorm != nullptr) *property = FNorm(*property);
    }

private:
    std::deque<T> valueDeq;
    std::deque<uint> timeDeq;
    bool running = false;
    uint timeTotal = 0u;
    uint timeLeft = 0u;
    T crntVal, nextVal;
};


namespace priv { inline float add_float(float _a, float _b) { return _a + _b; } }
using AnimatorFloat = Animator<float, priv::add_float, sq::maths::mix>;

using AnimatorVec2F = Animator<Vec2F, sq::operator+, sq::maths::mix>;
using AnimatorVec3F = Animator<Vec3F, sq::operator+, sq::maths::mix>;
using AnimatorVec4F = Animator<Vec4F, sq::operator+, sq::maths::mix>;
using AnimatorNormF = Animator<Vec3F, sq::operator+, sq::maths::mix, sq::maths::normalize>;
using AnimatorQuatF = Animator<QuatF, sq::operator*, sq::maths::slerp, sq::maths::normalize>;

}}
