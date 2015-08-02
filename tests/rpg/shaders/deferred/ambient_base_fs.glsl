// GLSL Fragment Shader

// define SSAO

in vec2 texcrd;

#include builtin/blocks/camera
#include headers/blocks/ambient

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform AMBIENTBLOCK { AmbientBlock LB; };

layout(binding=3) uniform sampler2D defrDiff;

#ifdef SSAO
layout(binding=0) uniform sampler2D texAmbOcc;
layout(binding=1) uniform sampler2D texDepHalf;
layout(binding=7) uniform sampler2D defrDepth;
#endif

out vec3 fragColour;


float linearise(float _depth) {
    const float n = CB.rmin, f = CB.rmax;
    return (2.f * n) / (f + n - _depth * (f - n)) * (f-n);
}


float nearest_depth_sample(vec2 _texcrd, sampler2D _depSmall, sampler2D _depLarge, sampler2D _sampler, float _thres) {
    vec4 txSmall = textureGather(_depSmall, texcrd);
    float txLarge = linearise(texture(_depLarge, _texcrd)).r;
    float distA = distance(txLarge, linearise(txSmall.r));
    float distB = distance(txLarge, linearise(txSmall.g));
    float distC = distance(txLarge, linearise(txSmall.b));
    float distD = distance(txLarge, linearise(txSmall.a));

    float value = 0.f;

    if (max(max(distA, distB), max(distC, distD)) > _thres) {
        float minDiff = CB.rmax;
        vec4 texel = textureGather(_sampler, _texcrd);
        if (distA < minDiff) { minDiff = distA; value = texel.r; }
        if (distB < minDiff) { minDiff = distB; value = texel.g; }
        if (distC < minDiff) { minDiff = distC; value = texel.b; }
        if (distD < minDiff) { minDiff = distD; value = texel.a; }
    } else value = texture(_sampler, texcrd).r;

    return value;
}

void main() {
    vec3 value = texture(defrDiff, texcrd).rgb;

    #ifdef SSAO
    float ssaoVal = 0.f;

    vec4 txSmall = textureGather(texDepHalf, texcrd);
    float txLarge = linearise(texture(defrDepth, texcrd)).r;
    float distA = distance(txLarge, linearise(txSmall.r));
    float distB = distance(txLarge, linearise(txSmall.g));
    float distC = distance(txLarge, linearise(txSmall.b));
    float distD = distance(txLarge, linearise(txSmall.a));

    if (max(max(distA, distB), max(distC, distD)) > 0.05f) {
        float minDiff = CB.rmax;
        vec4 texel = textureGather(texAmbOcc, texcrd);
        if (distA < minDiff) { minDiff = distA; ssaoVal = texel.r; }
        if (distB < minDiff) { minDiff = distB; ssaoVal = texel.g; }
        if (distC < minDiff) { minDiff = distC; ssaoVal = texel.b; }
        if (distD < minDiff) { minDiff = distD; ssaoVal = texel.a; }
    } else ssaoVal = texture(texAmbOcc, texcrd).r;

    value *= ssaoVal;
    #endif

    fragColour = value * LB.colour;
}
