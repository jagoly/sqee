#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec2 texcrd;

layout(binding=1) uniform sampler2D texPunch;


void main() {
    if (texture(texPunch, texcrd).a < 0.5f) discard;
}
