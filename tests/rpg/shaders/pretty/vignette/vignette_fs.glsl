#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec2 texcrd;

out vec4 fragColour;


void main() {
    float alpha = smoothstep(0.88f, 0.44f, distance(vec2(0.5f, 0.5f), texcrd));
    fragColour = vec4(0.f, 0.f, 0.f, 1.f - alpha);
}
