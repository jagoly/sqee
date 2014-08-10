#version 330

in vec2 texCoord;

layout(location = 0) out vec4 fragColour;

uniform sampler2D tex;

void main() {
    fragColour = texture(tex, texCoord);
}
