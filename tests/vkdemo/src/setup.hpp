#pragma once

#include <sqee/setup.hpp> // IWYU pragma: export

#include <sqee/core/TypeAliases.hpp> // IWYU pragma: export
#include <sqee/core/Utilities.hpp> // IWYU pragma: export

#include <sqee/maths/Vectors.hpp> // IWYU pragma: export
#include <sqee/maths/Matrices.hpp> // IWYU pragma: export
#include <sqee/maths/Quaternion.hpp> // IWYU pragma: export

namespace demo { //#############################################################

using namespace sq::coretypes;

namespace ranges = std::ranges;
namespace maths = sq::maths;

class DemoApp;
struct ResourceCaches;

} // namespace demo ############################################################
