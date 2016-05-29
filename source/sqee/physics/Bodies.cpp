#include <rp3d/engine/DynamicsWorld.hpp>
#include <rp3d/collision/shapes/BoxShape.hpp>
#include <rp3d/collision/shapes/SphereShape.hpp>
#include <rp3d/collision/shapes/ConeShape.hpp>
#include <rp3d/collision/shapes/CylinderShape.hpp>
#include <rp3d/collision/shapes/CapsuleShape.hpp>

#include <sqee/assert.hpp>
#include <sqee/redist/tinyformat.hpp>
#include <sqee/physics/Bodies.hpp>
#include <sqee/app/Resources.hpp>
#include <sqee/misc/Files.hpp>

using namespace sq;

// Constructors and Destructor /////

BaseBody::BaseBody(rp3d::DynamicsWorld& _world) : world(_world), body(world.createRigidBody({})) {}
StaticBody::StaticBody(rp3d::DynamicsWorld& _world) : BaseBody(_world) { body->setType(rp3d::BodyType::STATIC); }
DynamicBody::DynamicBody(rp3d::DynamicsWorld& _world) : BaseBody(_world) { body->setType(rp3d::BodyType::DYNAMIC); }
BaseBody::~BaseBody() { world.destroyRigidBody(body); }


// StaticBody Add Shape Methods /////

void StaticBody::add_BoxShape(uint _id, Vec3F _extent, Vec3F _pos, QuatF _rot) {
    SQASSERT(shapeMap.find(_id) == shapeMap.end(), "id already in use for this body");
    auto shape = (shapeMap[_id] = std::make_unique<rp3d::BoxShape>(rp3d_cast(_extent))).get();
    body->addCollisionShape(shape, rp3d_cast(_pos, _rot), INFINITY); }

void StaticBody::add_SphereShape(uint _id, float _radius, Vec3F _pos, QuatF _rot) {
    SQASSERT(shapeMap.find(_id) == shapeMap.end(), "id already in use for this body");
    auto shape = (shapeMap[_id] = std::make_unique<rp3d::SphereShape>(_radius)).get();
    body->addCollisionShape(shape, rp3d_cast(_pos, _rot), INFINITY); }

void StaticBody::add_ConeShape(uint _id, float _radius, float _height, Vec3F _pos, QuatF _rot) {
    SQASSERT(shapeMap.find(_id) == shapeMap.end(), "id already in use for this body");
    auto shape = (shapeMap[_id] = std::make_unique<rp3d::ConeShape>(_radius, _height)).get();
    body->addCollisionShape(shape, rp3d_cast(_pos, _rot), INFINITY); }

void StaticBody::add_CylinderShape(uint _id, float _radius, float _height, Vec3F _pos, QuatF _rot) {
    SQASSERT(shapeMap.find(_id) == shapeMap.end(), "id already in use for this body");
    auto shape = (shapeMap[_id] = std::make_unique<rp3d::CylinderShape>(_radius, _height)).get();
    body->addCollisionShape(shape, rp3d_cast(_pos, _rot), INFINITY); }

void StaticBody::add_CapsuleShape(uint _id, float _radius, float _height, Vec3F _pos, QuatF _rot) {
    SQASSERT(shapeMap.find(_id) == shapeMap.end(), "id already in use for this body");
    auto shape = (shapeMap[_id] = std::make_unique<rp3d::CapsuleShape>(_radius, _height)).get();
    body->addCollisionShape(shape, rp3d_cast(_pos, _rot), INFINITY); }


// DynamicBody Add Shape Methods /////

void DynamicBody::add_BoxShape(uint _id, Vec3F _extent, Vec3F _pos, QuatF _rot, float _mass) {
    SQASSERT(shapeMap.find(_id) == shapeMap.end(), "id already in use for this body");
    auto shape = (shapeMap[_id] = std::make_unique<rp3d::BoxShape>(rp3d_cast(_extent))).get();
    body->addCollisionShape(shape, rp3d_cast(_pos, _rot), _mass); }

void DynamicBody::add_SphereShape(uint _id, float _radius, Vec3F _pos, QuatF _rot, float _mass) {
    SQASSERT(shapeMap.find(_id) == shapeMap.end(), "id already in use for this body");
    auto shape = (shapeMap[_id] = std::make_unique<rp3d::SphereShape>(_radius)).get();
    body->addCollisionShape(shape, rp3d_cast(_pos, _rot), _mass); }

void DynamicBody::add_ConeShape(uint _id, float _radius, float _height, Vec3F _pos, QuatF _rot, float _mass) {
    SQASSERT(shapeMap.find(_id) == shapeMap.end(), "id already in use for this body");
    auto shape = (shapeMap[_id] = std::make_unique<rp3d::ConeShape>(_radius, _height)).get();
    body->addCollisionShape(shape, rp3d_cast(_pos, _rot), _mass); }

void DynamicBody::add_CylinderShape(uint _id, float _radius, float _height, Vec3F _pos, QuatF _rot, float _mass) {
    SQASSERT(shapeMap.find(_id) == shapeMap.end(), "id already in use for this body");
    auto shape = (shapeMap[_id] = std::make_unique<rp3d::CylinderShape>(_radius, _height)).get();
    body->addCollisionShape(shape, rp3d_cast(_pos, _rot), _mass); }

void DynamicBody::add_CapsuleShape(uint _id, float _radius, float _height, Vec3F _pos, QuatF _rot, float _mass) {
    SQASSERT(shapeMap.find(_id) == shapeMap.end(), "id already in use for this body");
    auto shape = (shapeMap[_id] = std::make_unique<rp3d::CapsuleShape>(_radius, _height)).get();
    body->addCollisionShape(shape, rp3d_cast(_pos, _rot), _mass); }


//template<typename... Args>
//void throw_error(const string& _path, int _lnum, const string& _msg, const Args&... _args) {
//    string message = tfm::format("Parsing SQP \"%s\"\nline %d: ", _path, _lnum);
//    throw std::runtime_error(message + tfm::format(_msg.c_str(), _args...));
//}

//void PhysObject::create(const string& _path) {
//    string path = static_path() + "physobjs/" + _path + ".sqp";
//    const auto fileVec = tokenise_file(path);

//    string section = "";
//    for (const auto& linePair : fileVec) {
//        const vector<string>& line = linePair.first;
//        const uint lnum = linePair.second;
//        const string& key = line.front();

//        if (key.front() == '#') continue;
//        if (key == "{") { if (section.empty()) section = line[1];
//            else throw_error(path, lnum, "Misplaced {"); continue; }
//        if (key == "}") { if (!section.empty()) section.clear();
//            else throw_error(path, lnum, "Misplaced }"); continue; }

//        if (section == "header") {
//            if (key == "type") {
//                if (line[1] == "DYNAMIC") bodyType = BodyType::DYNAMIC;
//                else if (line[1] == "STATIC") bodyType = BodyType::STATIC;
//                else throw_error(path, lnum, "Invalid type \"%s\"", line[1]);
//            }
//            else if (key == "lineardamp") lineardamp = stof(line[1]);
//            else if (key == "angulardamp") angulardamp = stof(line[1]);
//            else if (key == "bounciness") bounciness = stof(line[1]);
//            else if (key == "friction") friction = stof(line[1]);
//            else throw_error(path, lnum, "Invalid key \"%s\"", key);
//        }

//        else if (section == "shapes") {
//            if (key == "shape") {
//                if (line[1] == "Box") shapeVec.emplace_back(new BoxShape());
//                else if (line[1] == "Sphere") shapeVec.emplace_back(new SphereShape());
//                else throw_error(path, lnum, "Invalid shape \"%s\"", line[1]);
//            }
//            else if (key == "halfsize") {
//                if (shapeVec.back()->type != ShapeType::BOX)
//                    throw_error(path, lnum, "Property \"halfsize\" only valid for Box shapes");
//                static_cast<BoxShape*>(shapeVec.back().get())->halfsize = svtofv3(line, 1);
//            }
//            else if (key == "radius") {
//                if (shapeVec.back()->type != ShapeType::SPHERE)
//                    throw_error(path, lnum, "Property \"radius\" only valid for Sphere shapes");
//                static_cast<SphereShape*>(shapeVec.back().get())->radius = stof(line[1]);
//            }
//            else if (key == "offset") shapeVec.back()->offset = svtofv3(line, 1);
//            else if (key == "mass") shapeVec.back()->mass = stof(line[1]);
//            else throw_error(path, lnum, "Invalid shape property \"%s\"", key);
//        }

//        else throw_error(path, lnum, "Invalid section \"%s\"", section);
//    }
//}
