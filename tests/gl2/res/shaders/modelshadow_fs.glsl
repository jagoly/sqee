#version 330

in vec2 texCoord;

uniform sampler2D tex;

out vec4 fragColour;

void main() {
    vec4 texel = texture(tex, texCoord);

    fragColour = texel;
    //fragColour = vec4(1.f, 1.f, 1.f, 1.f);
}
