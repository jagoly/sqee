#pragma once
#include <sqee/forward.hpp>
#include <unordered_map>
#include <unordered_set>

#include <chaiscript/dispatchkit/boxed_value.hpp>
#include <chaiscript/dispatchkit/boxed_cast.hpp>
using chaiscript::Boxed_Value;
using chaiscript::boxed_cast;
#undef bool

namespace sqt {
namespace wcoe {

class Cell;
class World;

class ObjSpec {
public:
    ObjSpec(const string& _name);
    void parse_line(const vector<string>& _line);
    template <class T> void set_if(const string& _key, T& _ref) const;
    const string name;

private:
    template <class T> using TypeMap = std::unordered_map<string, T>;
    TypeMap<int> intMap; TypeMap<uint> uintMap; TypeMap<float> floatMap;
    TypeMap<fvec2> fvec2Map; TypeMap<fvec3> fvec3Map; TypeMap<fvec4> fvec4Map;
    TypeMap<uvec2> uvec2Map; TypeMap<uvec3> uvec3Map; TypeMap<uvec4> uvec4Map;
    TypeMap<ivec2> ivec2Map; TypeMap<ivec3> ivec3Map; TypeMap<ivec4> ivec4Map;
    TypeMap<fquat> fquatMap; TypeMap<string> stringMap;
    std::unordered_set<string> flagSet;
};


class Object : NonCopyable {
public:
    Object(const string& _name, Cell* _cell);
    virtual ~Object() = default;

    virtual void load_from_spec(const ObjSpec& _spec) = 0;
    virtual void refresh() = 0; virtual void tick() = 0;
    virtual void calc(double _accum) = 0;
    virtual void animate() = 0;

    const string name;

    const Cell* const cell;
    std::unordered_map<string, Boxed_Value> extra;

    bool check_invalid();
    void invalidate();

protected:
    const World* const world;
    const sq::Camera* const camera;
    const sq::Settings& settings;

private:
    bool invalid = true;
};

}}
