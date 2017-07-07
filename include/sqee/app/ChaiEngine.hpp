#pragma once

#include <memory>

#include <sqee/setup.hpp>

//============================================================================//

/// @cond ignored
namespace chaiscript { class ChaiScript; }
/// @endcond ignored

//============================================================================//

namespace sq {

//============================================================================//

/// Alias for chai::ChaiScript.
using ChaiEngine = chai::ChaiScript;

/// Create a ChaiEngine instance.
std::unique_ptr<ChaiEngine> create_ChaiEngine();

//============================================================================//

} // namespace sq
