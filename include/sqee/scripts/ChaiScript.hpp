#pragma once
#include <sqee/forward.hpp>

#include <chaiscript/chaiscript.hpp>

#undef bool

namespace chai = chaiscript;

namespace sq {

/// Create a ChaiScript instance
chai::ChaiScript* make_ChaiScript();

}
