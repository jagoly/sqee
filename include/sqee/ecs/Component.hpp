#pragma once

#include <sqee/builtins.hpp>

// Forward Declarations /////
namespace sq { class Entity; }

namespace sq {

class Component : sq::NonCopyable {
public:
    virtual ~Component() = default;

    virtual void setup_depends(Entity*) {}

    void mark_for_configure();
    void mark_for_refresh();

    vector<Component*> rDepends;
    bool needsConfigure = true;
    bool needsRefresh = true;
};

}
