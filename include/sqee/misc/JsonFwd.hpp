// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

// not useful for the way we handle errors
#define JSON_DIAGNOSTICS 0

// don't want to accidentally serialise enums as ints
#define JSON_DISABLE_ENUM_SERIALIZATION 0

// sqee doesn't use iostreams anywhere
#define JSON_NO_IO 1

// implicit conversions don't play nice with fmtlib
#define JSON_USE_IMPLICIT_CONVERSIONS 0

#include <sqee/redist/json_fwd.hpp> // IWYU pragma: export

// IWYU pragma: no_include <cstdint>
// IWYU pragma: no_include <map>
// IWYU pragma: no_include <string>
// IWYU pragma: no_include <vector>

namespace sq {

// use 32bit number types for compatibility with wren
using JsonValue = nlohmann::basic_json<std::map, std::vector, std::string, bool, int32_t, uint32_t, float>;

} // namespace sq

