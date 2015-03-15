#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec2 texcrd;

uniform sampler2D tex;

out vec4 fragColour;


vec3 tone_map(vec3 x) {
    float A = 0.15; float B = 0.50; float C = 0.10;
    float D = 0.20; float E = 0.02; float F = 0.30;
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main() {
    vec4 texel = texture(tex, texcrd);
    vec3 colour = tone_map(texel.rgb) / tone_map(vec3(1.f / texel.a));
    fragColour = vec4(colour, texel.a);
}
