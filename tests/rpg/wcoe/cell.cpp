#include <sqee/app/logging.hpp>
#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/misc/files.hpp>

#include "obj/modelstatic.hpp"
#include "obj/modelskelly.hpp"
#include "obj/spotlight.hpp"
#include "obj/pointlight.hpp"
#include "obj/liquid.hpp"
#include "obj/reflector.hpp"
#include "cell.hpp"

using namespace sqt::wcoe;

Cell::Cell(const string& _name, vec3 _position, const World* _world)
    : name(_name), position(_position), world(_world) {}

void Cell::load_from_file(const string& _path) {
    vector<vector<string>> fileVec(sq::get_words_from_file("assets/cells/" + _path + ".sq_cell"));
    vector<pair<string, ObjSpec>> specVec;

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
                if      (line[1] == "modelstatic") add_object<ModelStatic>(line[2]);
                else if (line[1] == "modelskelly") add_object<ModelSkelly>(line[2]);
                else if (line[1] == "spotlight")   add_object<SpotLight>(line[2]);
                else if (line[1] == "pointlight")  add_object<PointLight>(line[2]);
                else if (line[1] == "liquid")      add_object<Liquid>(line[2]);
                else if (line[1] == "reflector")   add_object<Reflector>(line[2]);
                else throw; // invalid object type
                specVec.emplace_back(line[2], ObjSpec());
            } else specVec.back().second.parse_line(line);
            continue;
        }
    }

    for (const auto& ss : specVec)
        get_object(ss.first).load_from_spec(ss.second),
        get_object(ss.first).update_from_data();
}

void Cell::tick() {
    for (auto& so : objectMap) so.second->tick();
}

void Cell::calc(double _accum) {
    for (auto& so : objectMap) so.second->calc(_accum);
}
