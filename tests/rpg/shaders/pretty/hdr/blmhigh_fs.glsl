#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec2 texcrd;

layout(binding=0) uniform sampler2D texMain;

out vec3 fragColour;


vec3 tone_map(vec3 _tx) {
    float A = 0.15f; float B = 0.50f; float C = 0.10f;
    float D = 0.20f; float E = 0.02f; float F = 0.30f;
    return ((_tx*(A*_tx+C*B)+D*E)/(_tx*(A*_tx+B)+D*F))-E/F;
}

void main() {
    vec4 texel = texture(texMain, texcrd);
    texel.rgb *= 0.666 * texel.a * texel.a * texel.a;
    fragColour = tone_map(texel.rgb) / tone_map(vec3(1.f / texel.a));
}
