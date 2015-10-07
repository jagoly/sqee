// GLSL Fragment Shader

// define BLOOM

in vec2 texcrd;

layout(binding=0) uniform sampler2D texMain;

#ifdef BLOOM
layout(binding=1) uniform sampler2D texBloom;
#endif

out vec4 fragColour;


vec3 tone_map(vec3 _tx) {
    float A = 0.15f; float B = 0.50f; float C = 0.10f;
    float D = 0.20f; float E = 0.02f; float F = 0.30f;
    return ((_tx*(A*_tx+C*B)+D*E)/(_tx*(A*_tx+B)+D*F))-E/F;
}

void main() {
    vec3 texel = texture(texMain, texcrd).rgb;
    float sqrtLuma = sqrt(dot(vec3(0.22f, 0.69f, 0.09f), texel));
    vec3 value = tone_map(texel.rgb) / tone_map(vec3(1.f / sqrtLuma));
//    value = texel.rgb;

    #ifdef BLOOM
    value += texture(texBloom, texcrd).rgb;
    #endif

    value = tone_map(value) / tone_map(vec3(1.f / sqrtLuma));
    fragColour = vec4(value, dot(vec3(0.22f, 0.69f, 0.09f), value));
}
