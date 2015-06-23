#pragma once
#include <sqee/forward.hpp>

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
    IntMap iMap; BoolMap bMap; FloatMap fMap; StringMap sMap;
    using FlagSet = unordered_set<string>; FlagSet flags;
    void parse_line(const vector<string>& _line);
};

class Object : NonCopyable {
public:
    Object(ObjType _type, const string& _name, Cell* _cell);
    virtual ~Object() = default;

    virtual void load_from_spec(const ObjSpec& _spec) = 0;
    virtual void refresh() = 0; virtual void tick() = 0;
    virtual void calc(double _accum) = 0;

    const ObjType type;
    const string name;
    Cell* const cell;
};

}}


#define SPEC_ASSERT_INT(Name, Count) \
    if (!_spec.iMap.count(Name)) throw std::runtime_error( "object \"" + name + "\" " \
         + "missing int[" + std::to_string(Count) + "] \"" + Name +"\""); \
    if (_spec.iMap.at(Name).size() != Count) throw std::runtime_error( "object \"" + name + "\" " \
         + "has bad size \"" + Name + "\", should be int[" + std::to_string(Count) + "]" );

#define SPEC_ASSERT_BOOL(Name, Count) \
    if (!_spec.bMap.count(Name)) throw std::runtime_error( "object \"" + name + "\" " \
         + "missing bool[" + std::to_string(Count) + "] \"" + Name +"\""); \
    if (_spec.bMap.at(Name).size() != Count) throw std::runtime_error( "object \"" + name + "\" " \
         + "has bad size \"" + Name + "\", should be bool[" + std::to_string(Count) + "]" );

#define SPEC_ASSERT_FLOAT(Name, Count) \
    if (!_spec.fMap.count(Name)) throw std::runtime_error( "object \"" + name + "\" " \
         + "missing float[" + std::to_string(Count) + "] \"" + Name +"\""); \
    if (_spec.fMap.at(Name).size() != Count) throw std::runtime_error( "object \"" + name + "\" " \
         + "has bad size \"" + Name + "\", should be float[" + std::to_string(Count) + "]" );

#define SPEC_ASSERT_STRING(Name, Count) \
    if (!_spec.sMap.count(Name)) throw std::runtime_error( "object \"" + name + "\" " \
         + "missing string[" + std::to_string(Count) + "] \"" + Name +"\""); \
    if (_spec.sMap.at(Name).size() != Count) throw std::runtime_error( "object \"" + name + "\" " \
         + "has bad size \"" + Name + "\", should be string[" + std::to_string(Count) + "]" );

#define SPEC_IF_INT(Name) if (_spec.iMap.count(Name))
#define SPEC_IF_BOOL(Name) if (_spec.bMap.count(Name))
#define SPEC_IF_FLOAT(Name) if (_spec.fMap.count(Name))
#define SPEC_IF_STRING(Name) if (_spec.sMap.count(Name))
#define SPEC_HAS_FLAG(Name) _spec.flags.count(Name) > 0u
