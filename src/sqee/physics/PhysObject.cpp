#include <sqee/misc/Files.hpp>
#include <sqee/physics/PhysObject.hpp>

using namespace sq;

void PhysObject::create(const string& _path) {
    string path = res::path() + "physobjs/" + _path + ".sqp";
    vector<vector<string>> fileVec = tokenise_file(path);

    string section = "";
    for (auto& line : fileVec) {
        const string& key = line[0]; if (key[0] == '#') continue;
        if (key == "{") { if (!section.empty()) throw; section = line[1]; continue; }
        if (key == "}") { if (section.empty()) throw; section.clear(); continue; }

        if (section == "header") {
            if (key == "type") {
                if (line[1] == "DYNAMIC") bodyType = BodyType::DYNAMIC;
                else if (line[1] == "STATIC") bodyType = BodyType::STATIC;
                else throw;
            }
            else if (key == "lineardamp") lineardamp = stof(line[1]);
            else if (key == "angulardamp") angulardamp = stof(line[1]);
            else if (key == "bounciness") bounciness = stof(line[1]);
            else if (key == "friction") friction = stof(line[1]);
            else throw;
        }

        else if (section == "shapes") {
            if (key == "shape") {
                if (line[1] == "Box") shapeVec.emplace_back(new BoxShape());
                else if (line[1] == "Sphere") shapeVec.emplace_back(new SphereShape());
                else throw;
            } else
            if (key == "halfsize") {
                if (shapeVec.back()->type != ShapeType::BOX) throw;
                static_cast<BoxShape*>(shapeVec.back().get())->halfsize = svtofv3(line, 1);
            } else
            if (key == "radius") {
                if (shapeVec.back()->type != ShapeType::SPHERE) throw;
                static_cast<SphereShape*>(shapeVec.back().get())->radius = stof(line[1]);
            }
            else if (key == "offset") shapeVec.back()->offset = svtofv3(line, 1);
            else if (key == "mass") shapeVec.back()->mass = stof(line[1]);
            else throw;
        }

        else throw;
    }

}

ResHolder<PhysObject>& sq::res::phys() {
    static ResHolder<PhysObject> holder;
    return holder;
}
