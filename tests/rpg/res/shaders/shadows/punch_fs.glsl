#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec2 texcrd;

layout(binding=1) uniform sampler2D texDiff;


void main() {
    if (texture(texDiff, texcrd).a < 0.5f) discard;
}
