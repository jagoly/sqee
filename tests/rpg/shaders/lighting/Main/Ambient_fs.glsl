// GLSL Fragment Shader

in vec2 texcrd;

#include runtime/Options
#include headers/blocks/Camera

uniform vec3 colour;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };

layout(binding=3) uniform sampler2D gbufDiff;

#ifdef OPTION_SSAO_ENABLE
#include builtin/funcs/depth
layout(binding=0) uniform sampler2D texAmbOcc;
layout(binding=1) uniform sampler2D texDepHalf;
layout(binding=7) uniform sampler2D gbufDepth;
#endif

out vec3 fragColour;


void main() {
    vec3 value = texture(gbufDiff, texcrd).rgb;

    #ifdef OPTION_SSAO_ENABLE
    const float near = OPTION_ViewNear; const float far = OPTION_ViewFar;
    value *= nearest_depth_sca(texcrd, texAmbOcc, gbufDepth, texDepHalf, 0.04f, near, far);
//    value *= texture(texAmbOcc, texcrd).r;
    #endif

    fragColour = value * colour;
}
