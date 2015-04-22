#include <sqee/app/logging.hpp>
#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/misc/files.hpp>

#include "obj/modelstatic.hpp"
#include "obj/modelskelly.hpp"
#include "obj/lightsky.hpp"
#include "obj/lightspot.hpp"
#include "obj/lightpoint.hpp"
#include "obj/liquid.hpp"
#include "obj/reflector.hpp"
#include "cell.hpp"

using namespace sqt::wcoe;

Cell::Cell(const string& _name, vec3 _position, const World& _world)
    : name(_name), position(_position), world(_world) {}

void Cell::load_from_file(const string& _path) {
    vector<vector<string>> fileVec(sq::get_words_from_file("assets/cells/" + _path + ".sq_cell"));
    struct SpecExtra { SpecExtra(string _name, ObjType _type) : name(_name), type(_type) {};
                       string name; ObjType type; ObjSpec spec; };
    vector<SpecExtra> specVec;

    string section = "";
    for (const auto& line : fileVec) {
        const string& key = line[0];
        if (key[0] == '#') continue;
        if (key == "{") { if (!section.empty()) throw; section = line[1]; continue; }
        if (key == "}") { if (section.empty()) throw; section.clear(); continue; }

        if (section == "header") {
            if (false) {} // empty
            else throw; // invalid key
            continue;
        }

        if (section == "objects") {
            if (key == "object") {
                ObjType type;
                if      (line[1] == "modelstatic") type = ObjType::ModelStatic;
                else if (line[1] == "modelskelly") type = ObjType::ModelSkelly;
                else if (line[1] == "lightsky")    type = ObjType::LightSky;
                else if (line[1] == "lightspot")   type = ObjType::LightSpot;
                else if (line[1] == "lightpoint")  type = ObjType::LightPoint;
                else if (line[1] == "liquid")      type = ObjType::Liquid;
                else if (line[1] == "reflector")   type = ObjType::Reflector;
                else throw; // invalid object type
                specVec.emplace_back(line[2], type);
            } else specVec.back().spec.parse_line(line);
            continue;
        }
    }

    for (const auto& spec : specVec) add_object(spec.name, spec.type, spec.spec);
}

void Cell::add_object(const string& _name, ObjType _type) {
    Object* ptr = nullptr;
    if      (_type == ObjType::ModelStatic) ptr = new ModelStatic(_name, *this);
    else if (_type == ObjType::ModelSkelly) ptr = new ModelSkelly(_name, *this);
    else if (_type == ObjType::LightSky)    ptr = new LightSky(_name, *this);
    else if (_type == ObjType::LightSpot)   ptr = new LightSpot(_name, *this);
    else if (_type == ObjType::LightPoint)  ptr = new LightPoint(_name, *this);
    else if (_type == ObjType::Liquid)      ptr = new Liquid(_name, *this);
    else if (_type == ObjType::Reflector)   ptr = new Reflector(_name, *this);
    objMap.emplace(_name, shared_ptr<Object>(ptr));
}

void Cell::add_object(const string& _name, ObjType _type, const ObjSpec& _spec) {
    add_object(_name, _type);
    objMap.at(_name)->load_from_spec(_spec);
}

void Cell::tick() {
    for (auto& so : objMap) so.second->tick();
}

void Cell::calc(double _accum) {
    for (auto& so : objMap) so.second->calc(_accum);
}
