#pragma once

#include <sqee/physics/RP3D.hpp>

#include "../Object.hpp"
#include "../Animation.hpp"

namespace sqt { namespace wcoe {

class ModelStatic;
class PointLight;
class SpotLight;
class Emitter;

class RigBodyBasic : public Object {
public:
    RigBodyBasic(const string& _name, Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void refresh(); void tick();
    void calc(double _accum);

    fvec3  PROP_scale    = {1.f, 1.f, 1.f};
    string PROP_physPath = "";

    void FUNC_set_ModelStatic(ModelStatic* _object);

    void animate();

    sq::PhysObject* phys = nullptr;
    rp3d::RigidBody* rigBody = nullptr;

private:
    Object* objectPtr = nullptr;
    fvec3* positionPtr = nullptr;
    fquat* rotationPtr = nullptr;
    fvec3 positionCrnt, positionNext;
    fquat rotationCrnt, rotationNext;
};

template<> struct ObjTraits<RigBodyBasic> {
    static constexpr ObjType type() { return ObjType::RigBodyBasic; }
};

}}
