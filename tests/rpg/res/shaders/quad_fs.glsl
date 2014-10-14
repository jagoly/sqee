#version 330

in vec2 texcoord;

uniform sampler2D screenTex;

out vec4 fragColour;

void main() {
    fragColour = texture(screenTex, texcoord);
}
