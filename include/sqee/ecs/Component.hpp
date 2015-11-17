#pragma once

#include <sqee/builtins.hpp>

// Forward Declarations /////
namespace sq { class Entity; }

namespace sq {

class Component : sq::NonCopyable {
public:
    virtual ~Component() = default;

    virtual void setup_depends(Entity*) {}

    void mark_for_update();

    vector<Component*> rDepends;
    bool needsUpdate = true;
};

}
