// GLSL Fragment Shader

in vec2 texcrd;

#include runtime/Options
#include builtin/funcs/depth

layout(binding=0) uniform sampler2D texShafts;
layout(binding=1) uniform sampler2D texDepHalf;

out vec3 fragColour;


const vec2 offsets[6] = {
vec2(-3.f, 0.f) * OPTION_PixSizeHalf,
vec2(-2.f, 0.f) * OPTION_PixSizeHalf,
vec2(-1.f, 0.f) * OPTION_PixSizeHalf,
vec2(+1.f, 0.f) * OPTION_PixSizeHalf,
vec2(+2.f, 0.f) * OPTION_PixSizeHalf,
vec2(+3.f, 0.f) * OPTION_PixSizeHalf
};


void main() {
    const float near = OPTION_ViewNear, far = OPTION_ViewFar;

    vec3 centreColour = fragColour = texture(texShafts, texcrd).rgb;
    float centreDepth = linearise(texture(texDepHalf, texcrd).r, near, far);

    for (int i = 0; i < 6; ++i) {
        vec2 sampCoord = texcrd + offsets[i];
        vec3 sampColour = texture(texShafts, sampCoord).rgb;
        float sampDepth = linearise(texture(texDepHalf, sampCoord).r, near, far);
        float factor = clamp(2.f - distance(centreDepth, sampDepth), 0.f, 1.f);
        fragColour += mix(centreColour, sampColour, factor);
    }

    fragColour /= 7.f;
}
