#pragma once

#include <sqee/app/application.hpp>

namespace sqt {

class RpgApp : public sq::Application {
public:
    RpgApp();
};

}

/* Uniform Buffer Layouts
 *
 * CameraBlock     44
 *   mat4 projMat  16
 *   mat4 viewMat  16
 *   vec3 camPos   4
 *   vec3 camRot   4
 *   vec2 zRange   4
 *
 * WorldBlock             292
 *   vec3 ambiColour      4
 *   bool useSkylight     4
 *   vec3 slDir           4
 *   vec3 slColour        4
 *   mat4 slShadMat       16
 *   uint spCount         4
 *   vec3 spPos[8]        32
 *   vec3 spDir[8]        32
 *   vec3 spColour[8]     32
 *   vec3 spFovIntSft[8]  32
 *   mat4 spShadMat[8]    128
 *
 *
 *
 * 2: LiquidBlock
 *
*/
