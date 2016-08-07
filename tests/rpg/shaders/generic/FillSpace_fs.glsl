// GLSL Fragment Shader

in vec2 texcrd;

#include runtime/Options

layout(binding=0) uniform sampler2D tex;

out vec3 fragColour;


const vec2 offsets[8] = {
vec2(-1.f, -1.f) * OPTION_PixSizeHalf,
vec2(-1.f,  0.f) * OPTION_PixSizeHalf,
vec2(-1.f, +1.f) * OPTION_PixSizeHalf,
vec2( 0.f, -1.f) * OPTION_PixSizeHalf,
vec2( 0.f, +1.f) * OPTION_PixSizeHalf,
vec2(+1.f, -1.f) * OPTION_PixSizeHalf,
vec2(+1.f,  0.f) * OPTION_PixSizeHalf,
vec2(+1.f, +1.f) * OPTION_PixSizeHalf
};

const vec3 SPACE_COLOUR = vec3(0.f, 0.f, 0.f);


void main() {
    float hitCount = 0.f;
    vec3 colour = vec3(0.f);
    for (int i = 0; i < 8; ++i) {
        vec3 texel = texture(tex, texcrd + offsets[i]).rgb;
        if (texel != SPACE_COLOUR) { colour += texel; hitCount += 1.f; }
    }

    if (hitCount == 0.f) fragColour = SPACE_COLOUR;
    else fragColour = colour / hitCount;
}
