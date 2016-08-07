#pragma once

#include <sqee/builtins.hpp>

namespace sq {

// Forward Declaration /////
class Entity;

class Component : sq::NonCopyable {
public:
    virtual ~Component() = default;
};

}
