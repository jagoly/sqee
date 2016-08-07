// GLSL Fragment Shader

in vec2 texcrd;

#include runtime/Options
#include builtin/funcs/depth
#include headers/blocks/Camera

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };

layout(binding=0) uniform sampler2D texAmbOcc;
layout(binding=1) uniform sampler2D texDepHalf;

out float fragColour;


const vec2 offsets[8] = {
vec2(-1.5f, -0.5f) * OPTION_PixSizeHalf,
vec2(-1.5f, +0.5f) * OPTION_PixSizeHalf,
vec2(-0.5f, -1.5f) * OPTION_PixSizeHalf,
vec2(-0.5f, +1.5f) * OPTION_PixSizeHalf,
vec2(+0.5f, -1.5f) * OPTION_PixSizeHalf,
vec2(+0.5f, +1.5f) * OPTION_PixSizeHalf,
vec2(+1.5f, -0.5f) * OPTION_PixSizeHalf,
vec2(+1.5f, +0.5f) * OPTION_PixSizeHalf,
};


void main() {
    const float near = OPTION_ViewNear, far = OPTION_ViewFar;

    float centreDepth = linearise(texture(texDepHalf, texcrd).r, near, far);
    float centreColour = fragColour = texture(texAmbOcc, texcrd).r;

    for (int i = 0; i < 8; ++i) {
        vec2 sampCoord = texcrd + offsets[i];
        float sampColour = texture(texAmbOcc, sampCoord).r;
        float sampDepth = linearise(texture(texDepHalf, sampCoord).r, near, far);
        float factor = smoothstep(distance(centreDepth, sampDepth), 0.01f, 0.02f);
        fragColour += mix(centreColour, sampColour, factor);
    }

    fragColour /= 9.f;
}
