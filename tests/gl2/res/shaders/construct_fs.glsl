#version 330

in vec2 texCoord;

uniform sampler2D tex;
uniform sampler2D texModshad;

out vec4 fragColour;

void main() {
    vec4 texel = texture(tex, texCoord);
    vec4 texelModshad = texture(texModshad, texCoord);

    fragColour = texel;// * (1.f - texelModshad.r);
    //fragColour = vec4(1.f, 1.f, 1.f, 1.f);
    //fragColour = vec4(1.f, 1.f, 1.f, 1.f);
}
