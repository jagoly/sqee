#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec2 texcoord;

uniform sampler2D screenTex;

out vec4 fragColour;


void main() {
    fragColour = texture(screenTex, texcoord);
}
