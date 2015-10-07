// GLSL Fragment Shader

in vec2 texcrd;

layout(binding=0) uniform sampler2D texMain;

out vec3 fragColour;


vec3 tone_map(vec3 _tx) {
    float A = 0.15f; float B = 0.50f; float C = 0.10f;
    float D = 0.20f; float E = 0.02f; float F = 0.30f;
    return ((_tx*(A*_tx+C*B)+D*E)/(_tx*(A*_tx+B)+D*F))-E/F;
}

void main() {
    vec3 texel = texture(texMain, texcrd).rgb;
    float luma = dot(vec3(0.22f, 0.69f, 0.09f), texel);

    vec3 value = texel * 0.333f * luma * luma * luma;
    fragColour = tone_map(value) / tone_map(vec3(1.f / luma));
}
