#pragma once
#include <sqee/forward.hpp>

namespace sqt { namespace wcoe {

enum class ObjType;
class ObjSpec;
class Object;
class World;

class Cell : NonCopyable {
public:
    Cell(const string& _name, vec3 _position, const World& _world);

    const string name;
    const vec3 position;
    const World& world;

    void load_from_file(const string& _path);
    void add_object(const string& _name, ObjType _type);
    void add_object(const string& _name, ObjType _type, const ObjSpec& _spec);

    void tick();
    void calc(double _accum);

    unordered_map<string, shared_ptr<Object>> objMap;
};

}}
