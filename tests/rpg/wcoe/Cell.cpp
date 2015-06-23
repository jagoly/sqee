#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/misc/Files.hpp>

#include "obj/ModelStatic.hpp"
#include "obj/ModelSkelly.hpp"
#include "obj/PointLight.hpp"
#include "obj/SpotLight.hpp"
#include "obj/Reflector.hpp"
#include "obj/Emitter.hpp"
#include "obj/Liquid.hpp"
#include "obj/Decal.hpp"
#include "Cell.hpp"

using namespace sqt::wcoe;

Cell::Cell(const string& _name, World* _world) : name(_name), world(_world) {}

void Cell::load_from_file(const string& _path) {
    vector<vector<string>> fileVec(sq::get_words_from_file("assets/cells/" + _path + ".sq_cell"));
    vector<pair<string, ObjSpec>> specVec;

    string section = "";
    for (const auto& line : fileVec) {
        const string& key = line[0]; if (key[0] == '#') continue;
        if (key == "{") { if (!section.empty()) throw; section = line[1]; continue; }
        if (key == "}") { if (section.empty()) throw; section.clear(); continue; }

        if (section == "header") { throw; }

        else if (section == "objects") {
            if (key == "object") {
                if      (line[1] == "modelstatic")     add_object<ModelStatic>(line[2]);
                else if (line[1] == "modelskelly")     add_object<ModelSkelly>(line[2]);
                else if (line[1] == "pointlight")      add_object<PointLight>(line[2]);
                else if (line[1] == "spotlight")       add_object<SpotLight>(line[2]);
                else if (line[1] == "reflector")       add_object<Reflector>(line[2]);
                else if (line[1] == "emitter")         add_object<Emitter>(line[2]);
                else if (line[1] == "liquid")          add_object<Liquid>(line[2]);
                else if (line[1] == "decal")           add_object<Decal>(line[2]);
                else throw; // invalid object type
                specVec.emplace_back(line[2], ObjSpec());
            } else specVec.back().second.parse_line(line);
            if (key == "flags") get_object(specVec.back().first)->
                                load_from_spec(specVec.back().second);
        }

        else throw;
    }
}

void Cell::refresh() {
    for (auto& so : objectMap) so.second->refresh();
}

void Cell::tick() {
    for (auto& so : objectMap) so.second->tick();
}

void Cell::calc(double _accum) {
    for (auto& so : objectMap) so.second->calc(_accum);
}
