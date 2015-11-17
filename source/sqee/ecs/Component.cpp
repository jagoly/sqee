#include <sqee/ecs/Component.hpp>

using namespace sq;

void Component::mark_for_update() {
    if (needsUpdate == false) { needsUpdate = true;
        for (auto rDep : rDepends) rDep->mark_for_update();
    }
}
