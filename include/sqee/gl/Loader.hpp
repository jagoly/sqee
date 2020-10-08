#pragma once

#include <sqee/export.hpp>

// An extremely simple function loader for loading Core Profile 4.5, and nothing else.
// There is no support for partial loading, if it fails to load any function, it gives up.
// Only Linux (GLX) and Windows (WGL) are supported, no Android or MacOS for now.

namespace sq {

/// Load OpenGL function pointers, return true on success.
SQEE_API bool load_opengl_core45_functions();

} // namespace sq
