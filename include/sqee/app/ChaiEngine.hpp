// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/misc/Builtins.hpp>

//============================================================================//

/// @cond ignore
namespace chaiscript { class ChaiScript; }
/// @endcond

//============================================================================//

namespace sq {

//============================================================================//

/// Alias for chai::ChaiScript.
using ChaiEngine = chaiscript::ChaiScript;

/// Create a ChaiEngine instance.
SQEE_API UniquePtr<ChaiEngine> create_ChaiEngine();

//============================================================================//

} // namespace sq
