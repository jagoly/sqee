// GLSL Fragment Shader

// define SSAO

in vec2 texcrd;

#include builtin/blocks/camera
#include headers/blocks/ambient

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform AMBIENTBLOCK { AmbientBlock LB; };

layout(binding=3) uniform sampler2D gbufDiff;

#ifdef SSAO
#include builtin/funcs/depth
layout(binding=0) uniform sampler2D texAmbOcc;
layout(binding=1) uniform sampler2D texDepHalf;
layout(binding=7) uniform sampler2D gbufDepth;
#endif

out vec3 fragColour;


void main() {
    vec3 value = texture(gbufDiff, texcrd).rgb;

    #ifdef SSAO
    value *= nearest_depth_sca(texcrd, texAmbOcc, gbufDepth, texDepHalf, 0.05f, CB.rmin, CB.rmax);
//    value *= texture(texAmbOcc, texcrd).r;
    #endif

    fragColour = value * LB.colour;
}
