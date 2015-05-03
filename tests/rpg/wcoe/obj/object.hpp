#pragma once
#include <sqee/forward.hpp>

namespace sqt { namespace wcoe {

class Cell;

enum class ObjType { ModelStatic, ModelSkelly,
                     LightSky, LightSpot, LightPoint,
                     Reflector, Liquid };

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


class Object : NonCopyable {
public:
    Object(ObjType _type, const string& _name, const Cell* _cell);
    virtual ~Object() = default;

    virtual void load_from_spec(const ObjSpec& _spec) = 0;
    virtual void update_from_data() = 0;

    const ObjType type;
    const string name;
    const Cell* const cell;

    virtual void tick() {}
    virtual void calc(double _accum) {}
};

}}
