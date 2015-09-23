#include <sqee/redist/tinyformat.hpp>
#include <sqee/physics/PhysObject.hpp>
#include <sqee/misc/Files.hpp>

using namespace sq;

template<typename... Args>
void throw_error(const string& _path, int _lnum, const string& _msg, const Args&... _args) {
    string message = tfm::format("Parsing SQP \"%s\"\nline %d: ", _path, _lnum);
    throw std::runtime_error(message + tfm::format(_msg.c_str(), _args...));
}


void PhysObject::create(const string& _path) {
    string path = res::path() + "physobjs/" + _path + ".sqp";
    const auto fileVec = tokenise_file(path);

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

        if (section == "header") {
            if (key == "type") {
                if (line[1] == "DYNAMIC") bodyType = BodyType::DYNAMIC;
                else if (line[1] == "STATIC") bodyType = BodyType::STATIC;
                else throw_error(path, lnum, "Invalid type \"%s\"", line[1]);
            }
            else if (key == "lineardamp") lineardamp = stof(line[1]);
            else if (key == "angulardamp") angulardamp = stof(line[1]);
            else if (key == "bounciness") bounciness = stof(line[1]);
            else if (key == "friction") friction = stof(line[1]);
            else throw_error(path, lnum, "Invalid key \"%s\"", key);
        }

        else if (section == "shapes") {
            if (key == "shape") {
                if (line[1] == "Box") shapeVec.emplace_back(new BoxShape());
                else if (line[1] == "Sphere") shapeVec.emplace_back(new SphereShape());
                else throw_error(path, lnum, "Invalid shape \"%s\"", line[1]);
            }
            else if (key == "halfsize") {
                if (shapeVec.back()->type != ShapeType::BOX)
                    throw_error(path, lnum, "Property \"halfsize\" only valid for Box shapes");
                static_cast<BoxShape*>(shapeVec.back().get())->halfsize = svtofv3(line, 1);
            }
            else if (key == "radius") {
                if (shapeVec.back()->type != ShapeType::SPHERE)
                    throw_error(path, lnum, "Property \"radius\" only valid for Sphere shapes");
                static_cast<SphereShape*>(shapeVec.back().get())->radius = stof(line[1]);
            }
            else if (key == "offset") shapeVec.back()->offset = svtofv3(line, 1);
            else if (key == "mass") shapeVec.back()->mass = stof(line[1]);
            else throw_error(path, lnum, "Invalid shape property \"%s\"", key);
        }

        else throw_error(path, lnum, "Invalid section \"%s\"", section);
    }

}

ResHolder<PhysObject>& sq::res::phys() {
    static ResHolder<PhysObject> holder;
    return holder;
}
