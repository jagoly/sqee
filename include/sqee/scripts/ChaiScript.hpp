#pragma once

#include <chaiscript/language/chaiscript_eval.hpp>
#include <chaiscript/language/chaiscript_engine.hpp>

#include <sqee/setup.hpp>

namespace sq {

/// Alias for chai::ChaiScript
using ChaiEngine = chai::ChaiScript;

/// Create a ChaiScript engine instance
std::unique_ptr<ChaiEngine> create_ChaiEngine();

}
