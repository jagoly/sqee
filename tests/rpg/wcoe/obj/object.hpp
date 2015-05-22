#pragma once
#include <sqee/forward.hpp>

#include <queue>

namespace sqt { namespace wcoe {

class Cell;

enum class ObjType { ModelStatic, ModelSkelly,
                     PointLight, SpotLight,
                     Reflector, Emitter,
                     Liquid, Decal };

template<class T> struct ObjTraits {
    template<T> static constexpr ObjType type();
};

struct ObjSpec {
    using IntMap = unordered_map<string, vector<int>>;
    using BoolMap = unordered_map<string, vector<bool>>;
    using FloatMap = unordered_map<string, vector<float>>;
    using StringMap = unordered_map<string, vector<string>>;
    using FlagSet = unordered_set<string>;

    IntMap iMap; BoolMap bMap; FloatMap fMap; StringMap sMap; FlagSet flags;
    void parse_line(const vector<string>& _line);
};


template<class T>
class Animator : NonCopyable {
public:
    Animator(T* _property, initializer_list<T> _values, initializer_list<uint> _times)
        : property(_property), values(_values), times(_times) {
        propCrnt = propNext = property;
    }

    void tick() {
        if (timeLeft == 0) {
            propCrnt = propNext;
            if (values.empty() || times.empty()) done = true;
            else { propNext = values.front(); values.pop();
                   timeTotal = times.front(); times.pop();
                   timeLeft = timeTotal; }
        } timeLeft -= 1u;
    }

    void calc(double _accum) {
        float percent = (double(timeLeft) + _accum*24.0) / double(timeTotal);
        *property = glm::mix(propCrnt, propNext, percent);
    }

    T* const property;
    std::queue<T> values;
    std::queue<uint> times;

    T propCrnt, propNext;
    uint timeLeft = 0u;
    uint timeTotal = 0u;
    bool done = false;
};


class Object : NonCopyable {
public:
    Object(ObjType _type, const string& _name, Cell* _cell);
    virtual ~Object() = default;

    virtual void load_from_spec(const ObjSpec& _spec) = 0;
    virtual void refresh() {}; virtual void tick() = 0;
    virtual void calc(double _accum) = 0;

    const ObjType type;
    const string name;
    Cell* const cell;
};


}}
