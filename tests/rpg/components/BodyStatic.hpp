#include <sqee/physics/Bodies.hpp>

#include "Helpers.hpp"

namespace sqt {

class StaticBodyComponent : public ComponentRPG
                          , public ecs::TagRefresh {
public:
    StaticBodyComponent(EntityRPG&, World&);
    static string type() { return "StaticBody"; }

    // System Properties /////
    sq::StaticBody body;
};

}
