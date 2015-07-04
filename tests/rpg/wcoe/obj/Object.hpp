#pragma once
#include <sqee/forward.hpp>

#include <exception>
#include <iostream>

namespace sqt { namespace wcoe {

class Cell;

enum class ObjType { RigBodyBasic, RigBodyMulti,
                     ModelStatic, ModelSkelly,
                     PointLight, SpotLight,
                     Reflector, Emitter,
                     Liquid, Decal };

template<class T> struct ObjTraits {
    template<T> static constexpr ObjType type();
};

struct ObjSpec {
    void parse_line(const vector<string>& _line);
    template<class T> using TypeMap = unordered_map<string, T>;
    TypeMap<int> intMap; TypeMap<uint> uintMap; TypeMap<float> floatMap;
    TypeMap<bool> boolMap; TypeMap<string> stringMap; TypeMap<fquat> fquatMap;
    TypeMap<fvec2> fvec2Map; TypeMap<fvec3> fvec3Map; TypeMap<fvec4> fvec4Map;
    TypeMap<uvec2> uvec2Map; TypeMap<uvec3> uvec3Map; TypeMap<uvec4> uvec4Map;
    TypeMap<ivec2> ivec2Map; TypeMap<ivec3> ivec3Map; TypeMap<ivec4> ivec4Map;
    unordered_set<string> flagSet;
};

class SpecException : public std::exception {
public:
    SpecException(const string& _key, const string& _type, const string& _object);
    const char* what() noexcept; string key, type, object;
private: string message;
};

class Object : NonCopyable {
public:
    Object(ObjType _type, const string& _name, Cell* _cell);
    virtual ~Object() = default;

    virtual void load_from_spec(const ObjSpec& _spec) = 0;
    virtual void refresh() = 0; virtual void tick() = 0;
    virtual void calc(double _accum) = 0;
    virtual void animate() = 0;

    const ObjType type;
    const string name;
    Cell* const cell;

};


inline void assert_int(const ObjSpec& _spec, const string& _object, const string& _key) {
    if (!_spec.intMap.count(_key)) throw SpecException(_key, "int", _object); }
inline void assert_uint(const ObjSpec& _spec, const string& _object, const string& _key) {
    if (!_spec.uintMap.count(_key)) throw SpecException(_key, "uint", _object); }
inline void assert_float(const ObjSpec& _spec, const string& _object, const string& _key) {
    if (!_spec.floatMap.count(_key)) throw SpecException(_key, "float", _object); }
inline void assert_bool(const ObjSpec& _spec, const string& _object, const string& _key) {
    if (!_spec.boolMap.count(_key)) throw SpecException(_key, "bool", _object); }
inline void assert_string(const ObjSpec& _spec, const string& _object, const string& _key) {
    if (!_spec.stringMap.count(_key)) throw SpecException(_key, "string", _object); }
inline void assert_fquat(const ObjSpec& _spec, const string& _object, const string& _key) {
    if (!_spec.fquatMap.count(_key)) throw SpecException(_key, "fquat", _object); }
inline void assert_ivec2(const ObjSpec& _spec, const string& _object, const string& _key) {
    if (!_spec.ivec2Map.count(_key)) throw SpecException(_key, "ivec2", _object); }
inline void assert_ivec3(const ObjSpec& _spec, const string& _object, const string& _key) {
    if (!_spec.ivec3Map.count(_key)) throw SpecException(_key, "ivec3", _object); }
inline void assert_ivec4(const ObjSpec& _spec, const string& _object, const string& _key) {
    if (!_spec.ivec4Map.count(_key)) throw SpecException(_key, "ivec4", _object); }
inline void assert_uvec2(const ObjSpec& _spec, const string& _object, const string& _key) {
    if (!_spec.uvec2Map.count(_key)) throw SpecException(_key, "uvec2", _object); }
inline void assert_uvec3(const ObjSpec& _spec, const string& _object, const string& _key) {
    if (!_spec.uvec3Map.count(_key)) throw SpecException(_key, "uvec3", _object); }
inline void assert_uvec4(const ObjSpec& _spec, const string& _object, const string& _key) {
    if (!_spec.uvec4Map.count(_key)) throw SpecException(_key, "uvec4", _object); }
inline void assert_fvec2(const ObjSpec& _spec, const string& _object, const string& _key) {
    if (!_spec.fvec2Map.count(_key)) throw SpecException(_key, "fvec2", _object); }
inline void assert_fvec3(const ObjSpec& _spec, const string& _object, const string& _key) {
    if (!_spec.fvec3Map.count(_key)) throw SpecException(_key, "fvec3", _object); }
inline void assert_fvec4(const ObjSpec& _spec, const string& _object, const string& _key) {
    if (!_spec.fvec4Map.count(_key)) throw SpecException(_key, "fvec4", _object); }

}}
