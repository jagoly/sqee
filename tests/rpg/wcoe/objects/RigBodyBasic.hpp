/*#pragma once

#include <sqee/forward.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>

#include "../Object.hpp"
#include "../Animation.hpp"

namespace reactphysics3d { class RigidBody; }

namespace sqt { namespace wcoe {

class ModelSimple;
class PointLight;
class SpotLight;
class Emitter;

class RigBodyBasic : public Object {
public:
    RigBodyBasic(const string& _name, Cell* _cell);

    void load_from_spec(const ObjSpec& _spec);

    void refresh(), update();
    void calc(double _accum);
    void animate();

    Vec3F  PROP_scale    = {1.f, 1.f, 1.f};
    string PROP_physobj  = "";

    void FUNC_set_ModelSimple(ModelSimple* _object);

    sq::PhysObject* physobj = nullptr;
    rp3d::RigidBody* rigBody = nullptr;

private:
    Object* objectPtr = nullptr;
    Vec3F* positionPtr = nullptr;
    QuatF* rotationPtr = nullptr;
    Vec3F positionCrnt, positionNext;
    QuatF rotationCrnt, rotationNext;
};

}}
*/
