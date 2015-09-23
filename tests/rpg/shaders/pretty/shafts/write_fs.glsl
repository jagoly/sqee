// GLSL Fragment Shader

in vec2 texcrd;

#include builtin/blocks/camera
#include headers/blocks/skylight

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform SKYLIGHTBLOCK { SkyLightBlock LB; };

layout(binding=0) uniform sampler2D texShafts;
layout(binding=1) uniform sampler2D texDepQter;
layout(binding=2) uniform sampler2D defrDepth;

out vec3 fragColour;


float linearise(float _depth) {
    const float n = CB.rmin, f = CB.rmax;
    return (2.f * n) / (f + n - _depth * (f - n)) * (f-n);
}

void main() {
    float value = 0.f;

    vec4 txSmall = textureGather(texDepQter, texcrd);
    float txLarge = linearise(texture(defrDepth, texcrd).r);
    float distA = distance(txLarge, linearise(txSmall.r));
    float distB = distance(txLarge, linearise(txSmall.g));
    float distC = distance(txLarge, linearise(txSmall.b));
    float distD = distance(txLarge, linearise(txSmall.a));

    if (max(max(distA, distB), max(distC, distD)) > 2.f) {
        float minDiff = CB.rmax;
        vec4 texel = textureGather(texShafts, texcrd);
        if (distA < minDiff) { minDiff = distA; value = texel.r; }
        if (distB < minDiff) { minDiff = distB; value = texel.g; }
        if (distC < minDiff) { minDiff = distC; value = texel.b; }
        if (distD < minDiff) { minDiff = distD; value = texel.a; }
    } else value = texture(texShafts, texcrd).r;

    float luma = dot(vec3(0.22f, 0.69f, 0.09f), LB.colour);
    fragColour = LB.colour / luma * value;
}
