#pragma once

#include <typeindex>
#include <unordered_map>
#include <unordered_set>

#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>

namespace sq { class Settings; }

namespace sqt { namespace wcoe {

class Cell; class World;

class ObjSpec {
public:
    ObjSpec(const string& _name);
    void parse_line(const vector<string>& _line);
    template <class T> void set_if(const string& _key, T& _ref) const;
    const string name;

private:
    template <class T> using TypeMap = std::unordered_map<string, T>;
    TypeMap<int> intMap; TypeMap<uint> uintMap; TypeMap<float> floatMap;
    TypeMap<Vec2I> ivec2Map; TypeMap<Vec3I> ivec3Map; TypeMap<Vec4I> ivec4Map;
    TypeMap<Vec2U> uvec2Map; TypeMap<Vec3U> uvec3Map; TypeMap<Vec4U> uvec4Map;
    TypeMap<Vec2F> fvec2Map; TypeMap<Vec3F> fvec3Map; TypeMap<Vec4F> fvec4Map;
    TypeMap<QuatF> fquatMap; TypeMap<string> stringMap;
    std::unordered_set<string> flagSet;
};


class Object : sq::NonCopyable {
public:
    Object(std::type_index _type, const string& _name, Cell* _cell);
    virtual ~Object() = default;

    virtual void load_from_spec(const ObjSpec& _spec) = 0;

    virtual void refresh() = 0, update() = 0;
    virtual void calc(double _accum) = 0;
    virtual void animate() = 0;

    const std::type_index type;
    const string name;
    const Cell* const cell;

    //chai::dispatch::Dynamic_Object extra;

    void invalidate();

protected:
    const World& world;
    const sq::Settings& settings;

    bool invalid = true;
};

}}
