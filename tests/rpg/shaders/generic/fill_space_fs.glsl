// GLSL Fragment Shader

// define PIXSIZE vec2
#define spaceColour vec3(0.f, 0.f, 0.f)

in vec2 texcrd;

layout(binding=0) uniform sampler2D tex;

out vec3 fragColour;


const vec2 offsets[8] = {
vec2(-1.f, -1.f),
vec2(-1.f,  0.f),
vec2(-1.f, +1.f),
vec2( 0.f, -1.f),
vec2( 0.f, +1.f),
vec2(+1.f, -1.f),
vec2(+1.f,  0.f),
vec2(+1.f, +1.f)
};


void main() {
    float hitCount = 0.f;
    vec3 colour = vec3(0.f);
    for (uint i = 0u; i < 8u; ++i) {
        vec3 texel = texture(tex, texcrd + offsets[i] * PIXSIZE).rgb;
        if (texel != spaceColour) { colour += texel; hitCount += 1.f; }
    }

    if (hitCount == 0.f) fragColour = spaceColour;
    else fragColour = colour / hitCount;
}
