#include <sqee/physics/Bodies.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>

#include "Helpers.hpp"

namespace sqt {

class DynamicBodyComponent : public ComponentRPG
                           , public ecs::TagUpdate
                           , public ecs::TagTick {
public:
    DynamicBodyComponent(EntityRPG&, World&);
    static string type() { return "DynamicBody"; }

    // System Properties /////
    sq::DynamicBody body;
    Vec3F prevPosition;
    QuatF prevRotation;

};

}
