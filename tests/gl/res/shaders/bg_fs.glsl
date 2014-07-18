#version 330

in vec3 colour;
out vec4 fragColour;

void main() {
    fragColour = vec4(colour, 0.4f);
}
