#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec2 texcrd;

layout(binding=0) uniform sampler2D tex;

out vec4 fragColour;


vec3 tone_map(vec3 _tx) {
    float A = 0.15f; float B = 0.50f; float C = 0.10f;
    float D = 0.20f; float E = 0.02f; float F = 0.30f;
    return ((_tx*(A*_tx+C*B)+D*E)/(_tx*(A*_tx+B)+D*F))-E/F;
}

void main() {
    vec4 texel = texture(tex, texcrd);
    vec3 colour = tone_map(texel.rgb) / tone_map(vec3(1.f / texel.a));
    fragColour = vec4(colour, texel.a);
}