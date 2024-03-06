#pragma once

#include <sqee/export.hpp>

#include <cstdint>

//==============================================================================

// cone scale = 1.00864

SQEE_API extern const float sqee_ConeVertices[26*3];
SQEE_API extern const uint8_t sqee_ConeIndices[48*3];

// sphere scale = 1.05146

SQEE_API extern const float sqee_SphereVertices[42*3];
SQEE_API extern const uint8_t sqee_SphereIndices[80*3];

// cube scale = 1.0

SQEE_API extern const float sqee_CubeVertices[8*3];
SQEE_API extern const uint8_t sqee_CubeIndices[12*3];
