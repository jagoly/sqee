#include <sqee/ecs/Component.hpp>

using namespace sq;

void Component::mark_for_configure() {
    needsConfigure = true;
    mark_for_refresh();
}

void Component::mark_for_refresh() {
    if (needsRefresh == false) { needsRefresh = true;
        for (auto rDep : rDepends) rDep->mark_for_refresh();
    }
}
