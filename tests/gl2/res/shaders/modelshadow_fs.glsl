#version 330

in vec2 texCoord;

layout(location = 0) out vec4 fragColour;

uniform sampler2D tex;

void main() {
    float shad = texture(tex, texCoord).r;
    fragColour = vec4(1.f, 1.f, 1.f, 1.f - shad);
    //fragColour = vec4(t_norm, 1.f);
    //fragColour = vec4(specTexel, 1.f);
    //fragColour = vec4(1.f, 1.f, 1.f, 1.f);
}
