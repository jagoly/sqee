#include <sqee/redist/tinyformat.hpp>
#include <sqee/misc/ResHolder.hpp>
#include <sqee/misc/Files.hpp>

#include "objects/MetaObject.hpp"
#include "objects/RigBodyBasic.hpp"
#include "objects/RigBodyMulti.hpp"
#include "objects/ModelSimple.hpp"
#include "objects/ModelSkelly.hpp"
#include "objects/PointLight.hpp"
#include "objects/SpotLight.hpp"
#include "objects/Reflector.hpp"
#include "objects/Emitter.hpp"
#include "objects/Liquid.hpp"
#include "objects/Decal.hpp"
#include "Cell.hpp"

using namespace sqt::wcoe;

template<typename... Args>
void throw_error(const string& _path, int _lnum, const string& _msg, const Args&... _args) {
    string message = tfm::format("Parsing SQ_CELL \"%s\"\nline %d: ", _path, _lnum);
    throw runtime_error(message + tfm::format(_msg.c_str(), _args...));
}


Cell::Cell(const string& _name, World& _world)
    : name(_name), world(_world) {}

void Cell::load_from_file(const string& _path) {
    string path = sq::res::path() + "cells/" + _path + ".sq_cell";
    const auto fileVec = sq::tokenise_file(path);

    vector<ObjSpec> specVec;

    string section = "";
    for (const auto& linePair : fileVec) {
        const vector<string>& line = linePair.first;
        const uint lnum = linePair.second;
        const string& key = line.front();

        if (key.front() == '#') continue;
        if (key == "{") { if (section.empty()) section = line[1];
            else throw_error(path, lnum, "Misplaced {"); continue; }
        if (key == "}") { if (!section.empty()) section.clear();
            else throw_error(path, lnum, "Misplaced }"); continue; }

        if (section == "objects") {
            if (key == "object") {
                if      (line[1] == "metaobject")    add_object<MetaObject>(line[2]);
                else if (line[1] == "rigbodybasic")  add_object<RigBodyBasic>(line[2]);
                else if (line[1] == "rigbodymulti")  add_object<RigBodyMulti>(line[2]);
                else if (line[1] == "modelsimple")   add_object<ModelSimple>(line[2]);
                else if (line[1] == "modelskelly")   add_object<ModelSkelly>(line[2]);
                else if (line[1] == "pointlight")    add_object<PointLight>(line[2]);
                else if (line[1] == "spotlight")     add_object<SpotLight>(line[2]);
                else if (line[1] == "reflector")     add_object<Reflector>(line[2]);
                else if (line[1] == "emitter")       add_object<Emitter>(line[2]);
                else if (line[1] == "liquid")        add_object<Liquid>(line[2]);
                else if (line[1] == "decal")         add_object<Decal>(line[2]);
                else throw_error(path, lnum, "Invalid object type \"%s\"", line[1]);
                specVec.emplace_back(ObjSpec(line[2]));
            } else specVec.back().parse_line(line);

            if (key == "flags") get_object(specVec.back().name)->
                                load_from_spec(specVec.back());
        }

        else throw_error(path, lnum, "Invalid section \"%s\"", section);
    }
}

void Cell::refresh() {
    for (auto& so : objectMap) so.second->refresh();
}

void Cell::update() {
    for (auto& so : objectMap) so.second->update();
}

void Cell::calc(double _accum) {
    for (auto& so : objectMap) so.second->calc(_accum);
}

void Cell::invalidate() {
    for (auto& so : objectMap) so.second->invalidate();
}
