#include <stdexcept>

#include <sqee/redist/tinyformat.hpp>
#include <sqee/misc/Files.hpp>

#include "Cell.hpp"
#include "World.hpp"
#include "Object.hpp"

using namespace sqt::wcoe;

ObjSpec::ObjSpec(const string& _name) : name(_name) {}

void ObjSpec::parse_line(const vector<string>& _line) {
    const string& key = _line.front();

    if (key == "flags")
        for (uint i = 1u; i < _line.size(); ++i)
            flagSet.emplace(_line[i]);

    else if (key == "int") intMap.emplace(_line[1], stoi(_line[2]));
    else if (key == "uint") uintMap.emplace(_line[1], stou(_line[2]));
    else if (key == "float") floatMap.emplace(_line[1], stof(_line[2]));
    else if (key == "ivec2") ivec2Map.emplace(_line[1], svtoiv2(_line, 2));
    else if (key == "ivec3") ivec3Map.emplace(_line[1], svtoiv3(_line, 2));
    else if (key == "ivec4") ivec4Map.emplace(_line[1], svtoiv4(_line, 2));
    else if (key == "uvec2") uvec2Map.emplace(_line[1], svtouv2(_line, 2));
    else if (key == "uvec3") uvec3Map.emplace(_line[1], svtouv3(_line, 2));
    else if (key == "uvec4") uvec4Map.emplace(_line[1], svtouv4(_line, 2));
    else if (key == "fvec2") fvec2Map.emplace(_line[1], svtofv2(_line, 2));
    else if (key == "fvec3") fvec3Map.emplace(_line[1], svtofv3(_line, 2));
    else if (key == "fvec4") fvec4Map.emplace(_line[1], svtofv4(_line, 2));
    else if (key == "fquat") fquatMap.emplace(_line[1], svtofeu(_line, 2));
    else if (key == "string") stringMap.emplace(_line[1], _line[2]);
    else throw std::runtime_error("Invalid type \""+ key +"\" when loading object \""+ name +"\"");
}

namespace sqt { namespace wcoe {

template<> void ObjSpec::set_if<bool>(const string& _key, bool& _ref) const {
    _ref = flagSet.count(_key);
}

#define OBJSPEC_SET_TEMPLATE(Type, Map) \
template<> void ObjSpec::set_if<Type>(const string& _key, Type& _ref) const { \
    if (Map.count(_key)) _ref = Map.at(_key); \
}

OBJSPEC_SET_TEMPLATE(int, intMap)
OBJSPEC_SET_TEMPLATE(uint, uintMap)
OBJSPEC_SET_TEMPLATE(float, floatMap)
OBJSPEC_SET_TEMPLATE(Vec2I, ivec2Map)
OBJSPEC_SET_TEMPLATE(Vec3I, ivec3Map)
OBJSPEC_SET_TEMPLATE(Vec4I, ivec4Map)
OBJSPEC_SET_TEMPLATE(Vec2U, uvec2Map)
OBJSPEC_SET_TEMPLATE(Vec3U, uvec3Map)
OBJSPEC_SET_TEMPLATE(Vec4U, uvec4Map)
OBJSPEC_SET_TEMPLATE(QuatF, fquatMap)
OBJSPEC_SET_TEMPLATE(Vec2F, fvec2Map)
OBJSPEC_SET_TEMPLATE(Vec3F, fvec3Map)
OBJSPEC_SET_TEMPLATE(Vec4F, fvec4Map)
OBJSPEC_SET_TEMPLATE(string, stringMap)

#undef OBJSPEC_GET_TEMPLATE

}}


Object::Object(std::type_index _type, const string& _name, Cell* _cell) : type(_type),
    name(_name), cell(_cell), world(_cell->world), settings(_cell->world.settings) {}

void Object::invalidate() { invalid = true; }
