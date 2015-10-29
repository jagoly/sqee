// GLSL Fragment Shader

// define SHAFTS
// define BLOOM

in vec2 texcrd;

layout(binding=0) uniform sampler2D texMain;

#ifdef SHAFTS
#include builtin/funcs/depth
#include builtin/blocks/camera
layout(binding=1) uniform sampler2D texShafts;
layout(binding=2) uniform sampler2D texDepQter;
layout(binding=3) uniform sampler2D gbufDepth;
layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
#endif

#ifdef BLOOM
layout(binding=4) uniform sampler2D texBloom;
#endif

out vec4 fragColour;


vec3 tone_map(vec3 _tx) {
    float A = 0.15f; float B = 0.50f; float C = 0.10f;
    float D = 0.20f; float E = 0.02f; float F = 0.30f;
    return ((_tx*(A*_tx+C*B)+D*E)/(_tx*(A*_tx+B)+D*F))-E/F;
}

void main() {
    vec3 value = texture(texMain, texcrd).rgb;

    #ifdef SHAFTS
    value += nearest_depth_sca(texcrd, texShafts, gbufDepth, texDepQter, 2.f, CB.rmin, CB.rmax);
    #endif

    float sqrtLuma = sqrt(dot(vec3(0.22f, 0.69f, 0.09f), value));
    value = tone_map(value) / tone_map(vec3(1.f / sqrtLuma));

    #ifdef BLOOM
    value += texture(texBloom, texcrd).rgb;
    #endif

    fragColour = vec4(value, dot(vec3(0.22f, 0.69f, 0.09f), value));
}
